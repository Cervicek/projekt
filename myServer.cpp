#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <time.h>
#include <fstream>
#include <sys/stat.h>
#include <dirent.h>
#include <stdint.h>
#include <float.h>
#include <climits>
#include <cstring>


#define BUFSIZE 1024

using namespace std;

/*string buildHTTP(string url, string type, string host,char *date, string *message){
    string msg(
            type + ": " + url + " HTTP/1.1\r\n"
            "Host: " + host + "\r\n"
            "Date: " + date + ""
            "Accept: " + "application/javascript\r\n"
            "Accept-Encoding: " + "identity\r\n"
            "Content-Type: " + "application/octet-stream\r\n"
            "Content-Length: " + "13292\r\n"
            "\r\n"
        );
    *message = msg;
    return *message;
}*/

string getHeader(string *text){
    string tmp = *text;
    string header;
    size_t found = tmp.find("\r\n\r\n");
    if(found != string::npos){
        header = tmp.substr(0, found);
        *text = tmp.substr(found+4);
        return header;
    }else{
        return "Unknown error.";
    }
}

string getOrderFromHeader(string header){
    string tmp = header;
    string type;
    size_t found = tmp.find(" ");
    if(found != string::npos){
        type = tmp.substr(0, found);
        return type;
    }else{
        return "Unknown error.";
    }
}

string getTypeFromHeader(string header){
    string tmp = header;
    string type;
    size_t found = tmp.find("=");
    size_t found2 = tmp.find("H");
    if(found != string::npos){
        type = tmp.substr(found+1, found2-found-2);
        return type;
    }else{
        return "Unknown error.";
    }
}

string getLenghtFromHeader(string header){
    string tmp = header;
    string lenght;
    size_t found = tmp.find_last_of(":");
    if(found != string::npos){
        lenght = tmp.substr(found+2);
        return lenght;
    }else{
        return "Unknown error.";
    }
}

string getPathFromHeader(string header){
    string tmp = header;
    string path;
    size_t found = tmp.find("?");
    size_t found2 = tmp.find("/");
    if(found != string::npos){
        path = tmp.substr(found2, found-found2);
        return path;
    }else{
        return "Unknown error.";
    }
}

string getNameFromPath(string path){
    string tmp = path;
    string name;
    size_t found = tmp.find_last_of("/");
    if(found != string::npos){
        name = tmp.substr(found);
        return path;
    }else{
        return "Unknown error.";
    }
}

int foundOutFileOrFolder(string name){
    string tmp = name;
    int decision;
    size_t found = tmp.find(".");
    if(found != string::npos){
        return 0;
    }else{
        return 1;
    }
}

string responseDEL(string *response, bool wrong, char *date, string err, int del){
    string http;
    if(wrong == true){
        http = "HTTP/1.1 404 Not Found";
    }else{
        http = "HTTP/1.1 200 OK";
    }

    string msg(
            http + "\r\n"
            "Date: "
        );
    msg.append(date);
    msg.append("Content-Type: application/octet-stream\r\n");
    if(wrong == true){
        msg.append("Content-Length: ");
        msg.append(to_string(del+12));
        msg.append("\r\nAccept-Encoding: identity\r\n\r\n");
        msg.append("{\"error\":\"");
        msg.append(err);
        msg.append("\"}");
    }else{
        msg.append("Content-Length: 26\r\nAccept-Encoding: identity\r\n\r\n");
        msg.append("{\"ok\":\"everything is ok\"}");
    }

    *response = msg;
    return *response;
}

string responsePUT(string *response, bool wrong, char *date, string err, int put){
    string http;
    if(wrong == true){
        http = "HTTP/1.1 400 Bad Request";
    }else{
        http = "HTTP/1.1 200 OK";
    }

    string msg(
            http + "\r\n"
            "Date: "
        );
    msg.append(date);
    msg.append("Content-Type: application/octet-stream\r\n");
    if(wrong == true){
        msg.append("Content-Length: ");
        msg.append(to_string(put+12));
        msg.append("\r\nAccept-Encoding: identity\r\n\r\n");
        msg.append("{\"error\":\"");
        msg.append(err);
        msg.append("\"}");
    }else{
        msg.append("Content-Length: 26\r\nAccept-Encoding: identity\r\n\r\n");
        msg.append("{\"ok\":\"everything is ok\"}");
    }

    *response = msg;
    return *response;
}

string responseGET(string *response, bool wrong, char *date, string ls, string err, int get, int n){
    string http;
    if(wrong == true){
        http = "HTTP/1.1 400 Bad Request";
    }else{
        http = "HTTP/1.1 200 OK";
    }

    string msg(
            http + "\r\n"
            "Date: "
        );
    msg.append(date);
    msg.append("Content-Type: application/octet-stream\r\n");
    if(wrong == true){
        msg.append("Content-Length: ");
        msg.append(to_string(get+12));
        msg.append("\r\nAccept-Encoding: identity\r\n\r\n");
        msg.append("{\"error\":\"");
        msg.append(err);
        msg.append("\"}");
    }else{
        msg.append("Content-Length: ");
        msg.append(to_string(n+9));
        msg.append("\r\nAccept-Encoding: identity\r\n\r\n");
        msg.append("{\"ls\":\"");
        msg.append(ls);
        msg.append("\"}");
    }

    *response = msg;
    //cout << *response << "ahoj" << endl;
    return *response;
}

string responseGetFile(string *response, bool wrong, char *date, char *buffer, size_t result, string err, int get){
    string http;
    //cout << wrong << err << endl;
    //cout << buffer << " asdasdasdd" << err << endl;
    if(wrong == true){
        http = "HTTP/1.1 400 Bad Request";
    }else{
        http = "HTTP/1.1 200 OK";
    }

    string msg(
            http + "\r\n"
            "Date: "
        );
    msg.append(date);
    msg.append("Content-Type: application/octet-stream\r\n");
    if(wrong == true){
        msg.append("Content-Length: ");
        msg.append(to_string(get+12));
        msg.append("\r\nAccept-Encoding: identity\r\n\r\n");
        msg.append("{\"error\":\"");
        msg.append(err);
        msg.append("\"}");
    }else{
        msg.append("Content-Length: ");
        msg.append(to_string(result));
        msg.append("\r\nAccept-Encoding: identity\r\n\r\n");
        msg.append(buffer, result);
        //msg.append("{\"ok\":\"everything is ok\"}");
    }


    *response = msg;
    return *response;
}

int main(int argc, char *argv[])
{
   std::string text;             // Přijímaný text
   sockaddr_in sockName;         // "Jméno" portu
   sockaddr_in clientInfo;       // Klient, který se připojil
   int mainSocket;               // Soket
   int port = 6677;              // Číslo portu
   char buf[BUFSIZE];            // Přijímací buffer
   int size;                     // Počet přijatých a odeslaných bytů
   socklen_t addrlen;            // Velikost adresy vzdáleného počítače
   unsigned long long int count = 0;                // Počet připojení
   int c;
   time_t rawtime;
    struct tm * timeinfo;
    char *date;
    string header;
    string type;
    string order;
    string lenght;
    string path;
    int lenghtINT;
    int decision;
    bool wrong = false;
    string response ;
    string message;
    DIR *Dir;
    struct dirent *DirEntry;
    string ls;
    string tmpLS;
    size_t foundLS;
    FILE * pFile;
    long lSize;
    char * buffer;
    size_t result;
    string err;
    string name;
    char *localPath;
    int put;
    int get;
    int del;
    int n = 0;

   while ((c = getopt (argc, argv, "r:p:")) != -1)
        switch (c)
        {
            case 'r':
                localPath = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case '?':
                if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
                return 1;
            default:
                abort ();
        }

    for (int index = optind; index < argc; index++){
        printf ("Non-option argument %s\n", argv[index]);
        return 1;
    }

   if ((mainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
   {
     fprintf(stderr,"Cannot create socket!\n");
     return -1;
   }
   sockName.sin_family = AF_INET;
   sockName.sin_port = htons(port);
   sockName.sin_addr.s_addr = INADDR_ANY;

   if (bind(mainSocket, (sockaddr *)&sockName, sizeof(sockName)) == -1)
   {
     fprintf(stderr,"Cannot name socket!\n");
     return -1;
   }

   if (listen(mainSocket, 10) == -1)
   {
     fprintf(stderr,"Cannot create queue!\n");
     return -1;
   }
   do
   {
     addrlen = sizeof(clientInfo);
     int client = accept(mainSocket, (sockaddr*)&clientInfo, &addrlen);
     int totalSize = 0;
     if (client == -1)
     {
       fprintf(stderr,"Connectivity problem!\n");
       return -1;
     }

     text = "";

    /* while (totalSize != BUFSIZE)
     {
       if ((size = read(client, buf, BUFSIZE)) == -1)
       {
            fprintf(stderr,"Cannot receive datas!\n");
            return -1;
       }
       totalSize += size;
       text.append(buf, size);
       if(text.find("\r\n\r\n")){
            break;
       }
     }*/

     while((size = read(client, buf, BUFSIZE)) > 0){
        text.append(buf, size);
        break;
     }

    header = getHeader(&text);
    //cout << header << endl;
    order = getOrderFromHeader(header);
    type = getTypeFromHeader(header);
    lenght = getLenghtFromHeader(header);
    path = getPathFromHeader(header);

     //time
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    date = asctime(timeinfo);
    //konec time

    if(!order.compare("PUT") && !type.compare("file")){
        lenghtINT = stoi(lenght);
        lenghtINT = lenghtINT - text.size();
        while(lenghtINT > 0 && (size = read(client, buf, BUFSIZE)) > 0){
            text.append(buf, size);
            lenghtINT = lenghtINT - size;
            if(lenghtINT == 0){
                break;
            }
        }
        if(FILE *file = fopen(path.c_str(), "r")){
            wrong = true;
            err = "Already exists.";
            put = 15;
            fclose(file);
        }else{
            ofstream myfile;
            myfile.open (path.c_str());
            if(!myfile.is_open()){
                wrong = true;
            }else{
                wrong = false;
                myfile << text;
                myfile.close();
            }
        }
        response = responsePUT(&response, wrong, date, err, put);
    }else if(!order.compare("PUT") && !type.compare("folder")){
        if((mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) != 0){
            wrong = true;
            err = "Already exists.";
            put = 15;
        }else{
            wrong =false;
        }
        response = responsePUT(&response, wrong, date, err, put);
    }else if(!order.compare("GET") && !type.compare("file")){
        //cout << err << endl;
        name = getNameFromPath(path);
        decision = foundOutFileOrFolder(name);
        if (decision == 0) {
            pFile = fopen ( path.c_str() , "rb" );
            if (pFile == NULL) {
                wrong = true;
                err = "File not found.";
                get = 15;
                response = responseGetFile(&response, wrong, date, buffer, result, err, get);
                err = "";
            }else{
                wrong = false;

                fseek (pFile , 0 , SEEK_END);
                lSize = ftell (pFile);
                rewind (pFile);
                //cout << buffer << "buffer" << endl;
                buffer = (char*) malloc (sizeof(char)*lSize);
                if (buffer == NULL) {
                    fputs ("Memory error",stderr);
                    exit (2);
                }

                result = fread (buffer,1,lSize,pFile);
                if (result != lSize) {fputs ("Reading error",stderr);
                exit (3);
                }
                //cout << buffer << "buffer" << endl;
                response = responseGetFile(&response, wrong, date, buffer, result, err, get);
                //cout << err << endl;
                //cout << response << endl;
                fclose (pFile);
                free (buffer);
            }
        }else{
            wrong = true;
            err = "Not a file.";
            del = 11;
            response = responseDEL(&response, wrong, date, err, del);
            err = "";
        }
        //cout << response << "bya" << endl;
    }else if(!order.compare("GET") && !type.compare("folder")){
        Dir = opendir(path.c_str());
        if (Dir){
            wrong = false;
            while((DirEntry=readdir(Dir)) != NULL)
            {
               tmpLS = DirEntry->d_name;
               foundLS = tmpLS.find_first_of(".");
               if(foundLS > 0){
                    if(ls.compare("")){
                        ls = ls + "\t" + DirEntry->d_name;
                    }else{
                        ls = DirEntry->d_name;
                    }
               }
            }
            n = ls.length();
            //cout << n << endl;
        }
        else if (ENOENT == errno){
            wrong = true;
            err ="Directory not found.";
            get = 20;
            ls = "";
        }else{
            wrong = true;
            err = "Not a directory.";
            get = 16;
            ls = "";
        }
        response = responseGET(&response, wrong, date, ls, err, get, n);
        ls = "";
        //cout << response << endl;
    }else if(!order.compare("DELETE") && !type.compare("file")){
        name = getNameFromPath(path);
        decision = foundOutFileOrFolder(name);
        if (decision == 0) {
            if( remove(path.c_str()) != 0 ){
                wrong = true;
                err = "File not found.";
                del = 15;
            }else{
                wrong =false;
            }
            response = responseDEL(&response, wrong, date, err, del);
        }else{
            wrong = true;
            err = "Not a file.";
            del = 11;
            response = responseDEL(&response, wrong, date, err, del);
        }
    }else if(!order.compare("DELETE") && !type.compare("folder")){
        name = getNameFromPath(path);
        decision = foundOutFileOrFolder(name);
        if (decision == 0) {
            wrong = true;
            err = "Not a directory.";
            del = 16;
            response = responseDEL(&response, wrong, date, err, del);
        }else{
            if(rmdir(path.c_str()) != 0){
                wrong = true;
                if(errno == 2){
                    err ="Directory not found.";
                    del = 20;
                }else{
                    err ="Directory not empty.";
                    del = 20;
                }
            }else{
                wrong =false;
            }
            response = responseDEL(&response, wrong, date, err, del);
        }
    }
    //cout << response << endl;
    //string message = buildHTTP("url", "cmd", "host", date, &text);
     if ((size = send(client, response.c_str(), response.size(), 0)) == -1)
     {
       fprintf(stderr,"Sending problem!\n");
       return -1;
     }
     close(client);
   }
   while (++count != ULLONG_MAX);
   close(mainSocket);
   return 0;
}
