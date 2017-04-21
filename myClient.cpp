#include <iostream>
#include <string>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <fstream>
#include <cstring>

#define BUFSIZE 1024
//#define LENGTH 512

using namespace std;

string buildHTTP(string url, string type, string host, char *date, string *message, string path, bool isLst, bool isFIle){
    //cout << isFIle << " v HTTP" << endl;
    int found;
    int foundURL;
    int foundDOT;
    while((found = path.find("/")) != string::npos){
        string cmp = path.substr(found+1);
        path = cmp;
    }

    int tmp = url.find("http://");
    if(tmp > -1){
        url = url.substr(7);
    }
    if((foundURL = url.find("/")) != string::npos){
        url = url.substr(foundURL);
    }
    if((foundDOT = path.find(".")) != string::npos && path.compare("")){
        //url = url + "/" + path + "?type=file";
        url = url + "?type=file";
    }else if((foundDOT = path.find(".")) == string::npos && path.compare("")){
        url = url + "?type=folder";
    }else if((foundDOT = url.find(".")) != string::npos && !path.compare("") && isLst == false){
        url = url + "?type=file";
    }else if((foundDOT = url.find(".")) == string::npos && !path.compare("") && isFIle == false){
        url = url + "?type=folder";
    }else if((foundDOT = url.find(".")) != string::npos && !path.compare("") && isLst == true){
        url = url + "?type=folder";
    }else if((foundDOT = url.find(".")) == string::npos && !path.compare("") && isFIle == true){
        url = url + "?type=file";
    }

    string msg(
            type + " " + url + " HTTP/1.1\r\n"
            "Host: " + host + "\r\n"
            "Date: " + date + ""
            "Accept: " + "application/javascript\r\n"
            "Accept-Encoding: " + "identity\r\n"
            //"\r\n"
        );

    if(path.compare("")){
        FILE * pFile;
        long lSize;
        char * buffer;
        size_t result;

        pFile = fopen ( path.c_str() , "rb" );
        if (pFile==NULL) {fputs ("Unknown error.\n",stderr); exit (1);}

        fseek (pFile , 0 , SEEK_END);
        lSize = ftell (pFile);
        rewind (pFile);

        buffer = (char*) malloc (sizeof(char)*lSize);
        if (buffer == NULL) {fputs ("Unknown error.\n",stderr); exit (2);}

        result = fread (buffer,1,lSize,pFile);
        if (result != lSize) {fputs ("Unknown error.\n",stderr); exit (3);}

        msg.append("Content-Type: application/octet-stream\r\nContent-Length: ");
        msg.append(to_string(result) + "\r\n");
        msg.append("\r\n");
        msg.append(buffer, result);


        fclose (pFile);
        free (buffer);
        //return 0;
    }else{
        msg.append("\r\n");
    }
    *message = msg;
    return *message;
}

int getCMD(string cmd, string *cmdHTTP){
    if((cmd) == "del"){
        (*cmdHTTP) = "DELETE";
        return 0;
    }
    else if((cmd) == "get"){
        (*cmdHTTP) = "GET";
        return 0;
    }
    else if((cmd) == "put"){
        (*cmdHTTP) = "PUT";
        return 0;
    }
    else if((cmd) == "lst"){
        (*cmdHTTP) = "GET";
        return 0;
    }
    else if((cmd) == "mkd"){
        (*cmdHTTP) = "PUT";
        return 0;
    }
    else if((cmd) == "rmd"){
        (*cmdHTTP) = "DELETE";
        return 0;
    }
    else{
        return -1;
    }
}

int getPORT(string text, string *portSTR){
    int found = text.find(":");
    int found2 = text.find("/");

    if(found == (found2-1)){
        *portSTR = "6677";
        return 2;
    }

    if(found > -1 && found2 > -1) {
        string cmp = text.substr(found+1, found2-found-1);
        *portSTR = cmp;
        return 0;
    }
    else{
        *portSTR = "6677";
        return 0;
    }
    return 0;
}

int getURL(string text, string *urlSTR){
    int found1 = text.find(":");
    int found2 = text.find("/");

    if(found1 > -1){
        string cmp = text.substr(0, found1);
        *urlSTR = cmp;
        return 0;
    }else if(found2 > -1 && found1 == -1){
        string cmp = text.substr(0, found2);
        *urlSTR = cmp;
        return 0;
    }else{
        return 2;
    }
    return 0;
}

int getROOT(string text, string *rootSTR){
    int found = text.find("/");
    if(found > -1){
        string cmp = text.substr(found);
        *rootSTR = cmp;
        return 0;
    }else{
        return 2;
    }
    return 0;
}

string getResponse(string text){
    string tmp = text;
    size_t found = tmp.find(" ");
    size_t found2 = tmp.find("\r\n");

    if(found != string::npos){
        text = tmp.substr(found+1, found2-found-1);
        return text;
    }else{
        return "Unknown error.\n";
    }
}

string getErrResponse(string text){
    string tmp = text;
    size_t found = tmp.find("{");
    if(found != string::npos){
        tmp = tmp.substr(found+1);
        size_t found2 = tmp.find(":");
        size_t found3 = tmp.find("}");
        if(found2 != string::npos){
            text = tmp.substr(found2+2, found3-found2-3);
           return text;
        }
        return text="";
    }else{
        return text ="";
    }
}

string getHeader(string *text){
    string tmp = *text;
    string header;
    size_t found = tmp.find("\r\n\r\n");
    if(found != string::npos){
        header = tmp.substr(0, found);
        *text = tmp.substr(found+4);
        return header;
    }else{
        return "Unknown error.\n";
    }
}

string getLenghtFromHeader(string header){
    string tmp = header;
    string lenght;
    size_t found = tmp.find_last_of("h");
    if(found != string::npos){
        lenght = tmp.substr(found+3);
        return lenght;
    }else{
        return "Unknown error.\n";
    }
}

string getNameInUrl(string urlFULL){
    string tmp = urlFULL;
    size_t foundNameInUrl = tmp.find_last_of("/");
    if(foundNameInUrl != string::npos){
        urlFULL = tmp.substr(foundNameInUrl+1);
        return urlFULL;
    }else{
        return "Unknown error.\n";
    }
}

int main(int argc, char *argv[])
{
    string text;      // Odesílaný a přijímaný text
    hostent *host;              // Vzdálený počítač;
    sockaddr_in serverSock;     // Vzdálený "konec potrubí"
    int mySocket;               // Soket
    int port;                   // Číslo portu
    char buf[BUFSIZE];          // Přijímací buffer
    int size;                   // Počet přijatých a odeslaných bytů
    int sockfd;
    string cmdHTTP;
    int cmd;
    int intPort;
    int url;
    int root;
    string tmpCMD;
    string tmpText;
    string portSTR;
    string urlSTR;
    string rootSTR;
    string urlFULL;
    time_t rawtime;
    struct tm * timeinfo;
    char *date;
    size_t result;
    int fileRead;
    FILE *pFile;
    char *buffer;
    string path;
    bool isLst = false;
    bool isFIle = false;
    string err;
    string header;
    string headerLength;
    int hLength;


    if(argc == 3){
        tmpCMD = argv[1];
        if(tmpCMD == "lst" || tmpCMD == "rmd"){
            isLst = true;
            //cout << isLst << endl;
        }
        if(tmpCMD == "del" || tmpCMD == "get"){
            isFIle = true;
            //cout << isFIle << endl;
        }
        tmpText = argv[2];
        urlFULL = tmpText;
        int tmp = tmpText.find("http://");
        if(tmp > -1){
            tmpText = tmpText.substr(7);
        }

        if((cmd = getCMD(tmpCMD, &cmdHTTP)) != 0){
            fprintf(stderr, "Unknown error.\n");
            return -1;
        }

        if((intPort = getPORT(tmpText, &portSTR)) != 0){
            fprintf(stderr, "Unknown error.\n");
            return -1;
        }

        if((url = getURL(tmpText, &urlSTR)) != 0){
            fprintf(stderr, "Unknown error.\n");
            return -1;
        }else{
            char *urlChar = new char [urlSTR.length() + 1];
            strcpy(urlChar, urlSTR.c_str());
            if ((host = gethostbyname(urlChar)) == NULL)
                {
                    fprintf(stderr,"Unknown error.\n");
                    return -1;
                }
        }

        if((root = getROOT(tmpText, &rootSTR)) != 0){
            fprintf(stderr, "Unknown error.\n");
            return -1;
        }

    }else if(argc == 4){
        tmpCMD = argv[1];
        tmpText = argv[2];
        path = argv[3];
        urlFULL = tmpText;
        int tmp = tmpText.find("http://");
        if(tmp > -1){
            tmpText = tmpText.substr(7);
        }

        if((cmd = getCMD(tmpCMD, &cmdHTTP)) != 0){
            fprintf(stderr, "Unknown error.\n");
            return -1;
        }

        if((intPort = getPORT(tmpText, &portSTR)) != 0){
            fprintf(stderr, "Unknown error.\n");
            return -1;
        }

        if((url = getURL(tmpText, &urlSTR)) != 0){
            fprintf(stderr, "Unknown error.\n");
            return -1;
        }else{
            char *urlChar = new char [urlSTR.length() + 1];
            strcpy(urlChar, urlSTR.c_str());
            if ((host = gethostbyname(urlChar)) == NULL)
                {
                    fprintf(stderr,"Unknown error.\n");
                    return -1;
                }
        }

        if((root = getROOT(tmpText, &rootSTR)) != 0){
            fprintf(stderr, "Unknown error.\n");
            return -1;
        }
    }else{
        fprintf(stderr, "Unknown error.\n");
        return -1;
    }

    /*cout << cmdHTTP << endl;
    cout << urlSTR << endl;
    cout << portSTR << endl;
    cout << rootSTR << endl;*/
    port = stoi(portSTR);

    if ((mySocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        fprintf(stderr,"Unknown error.\n");
        return -1;
    }

    //time
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    date = asctime(timeinfo);
    //konec time

    serverSock.sin_family = AF_INET;
    serverSock.sin_port = htons(port);
    memcpy(&(serverSock.sin_addr), host->h_addr, host->h_length);

    if (connect(mySocket, (sockaddr *)&serverSock, sizeof(serverSock)) == -1)
    {
        fprintf(stderr,"Unknown error.\n");
        return -1;
    }

    string message = buildHTTP(urlFULL, cmdHTTP, urlSTR, date, &text, path, isLst, isFIle);
    //cout << message << endl;
    if ((size = send(mySocket, message.c_str(), message.size(), 0)) == -1)
    {
        cerr << "Unknown error.\n" << endl;
        return -1;
    }

    text = "";
    while ((size = read(mySocket, buf, BUFSIZE)) > 0)
    {
        text.append(buf, size);
        //cout << text << endl;
        break;
    }

    //cout << sizeof(buf) << endl;
    //cout << text << endl;

    header = getHeader(&text);

    if(tmpCMD == "get"){
    	headerLength = getLenghtFromHeader(header);
    	hLength = stoi(headerLength);
    	hLength = hLength - text.size();
        string nameFile = getNameInUrl(urlFULL);
        while(hLength > 0 && (size = read(mySocket, buf, BUFSIZE)) > 0){
            text.append(buf, size);
            hLength = hLength - size;
            if(hLength == 0){
                break;
            }
        }
        ofstream myfile;
        myfile.open (nameFile.c_str());
        myfile << text;
        myfile.close();
    }
    string response = getResponse(header);
    //cout << response <<endl;
    err = getErrResponse(text);
    if(err != ""){
        size_t foundLS = err.find("\t");
        if(foundLS != string::npos && response == "200 OK"){
            cout << err <<endl;
        }else if(response != "200 OK"){
            cerr << err << endl;
        }
    }
    //cout << "ajaja123" << endl;
    close(mySocket);
    //fclose (pFile);
    //free (buffer);
    return 0;
}
