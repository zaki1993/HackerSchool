#include <iostream>
#include <string>
#include <cstring>      
#include <sys/socket.h> //for socket connection, etc.
#include <netdb.h>      
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h> 
#include <fstream> //reading and writing to files
#include <dirent.h> //get all the folder content
#include <sys/stat.h>
#include <math.h>//used in intToString function
#include <pthread.h>//for multythreading
using namespace std;

//declaring some variables
const int buffersize = 4096;
char incomming_data_buffer[buffersize];
ssize_t bytes_sent = 0;
std::string fullPath;
ssize_t bytes_recieved = 0;
ssize_t totalBytesRead = 0;
std::string addrcontainer = "";
int totalClients = 0;

    const char *response_200 = "HTTP/1.0 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n\r\n<html><body><i></i></body></html>";
    const char *response_400 = "HTTP/1.0 400 Bad Request\r\nContent-Type: text/html; charset=utf-8\r\n\r\n<html><body><i>Bad Request!</i></body></html>";
    const char *response_404 = "HTTP/1.0 404 Not Found\r\nContent-Type: text/html; charset=utf-8\r\n\r\n<html><body><i>Not Found!</i></body></html>";
    const char *response_408 = "HTTP/1.0 408 Request Timeout\r\nContent-Type: text/html; charset=utf-8\r\n\r\n<html><body><i>Request Timeout</i></body></html>";

typedef enum {gif, html, jpeg, jpg, png, plain,mp4, notfound} ext; //all the extensions that the web server recognises

//function that finds the extension of the file
ext get_ext(char *file) {
    if (strstr(file, ".gif") != NULL)
        return gif;
    if (strstr(file, ".html") != NULL)
        return html;
    if (strstr(file, ".jpeg") != NULL)
        return jpeg;
    if (strstr(file, ".jpg") != NULL)
        return jpg;
    if (strstr(file, ".png") != NULL)
        return png;
    if (strstr(file, ".txt") != NULL)
        return plain;
    if(strstr(file, ".mp4") != NULL)
	return mp4;
    else{
	return notfound;
    }
}
bool checkBytesError(ssize_t &num){
	if(num==-1){
		return true;
	}
	return false;
}
void sendResponse(int &socket,const char *request){
	bytes_sent = write(socket,request,strlen(request));
	if(checkBytesError(bytes_sent)){std::cout<<"Error sending response..!"<<std::endl; return;}
}
void NotFoundErr(int &cliSocket,ssize_t sent_bytes){ //404 NOT FOUND
	bytes_sent = write(cliSocket,response_404,strlen(response_404));
	if(checkBytesError(bytes_sent)){std::cout<<"Error sending response..!"<<std::endl; return;}
}
void getFolderContent(std::string path,int &cliSocket){
	std::ifstream fin;
	int num;
	struct dirent *dirp;
	struct stat filestat;
	std::string filepath;
	DIR *dp;
	//open
	dp = opendir(path.c_str());
	//read
	while((dirp = readdir(dp))){
		filepath = path + "/" + dirp->d_name;
		if (stat( filepath.c_str(), &filestat )) continue;
	    	if (S_ISDIR( filestat.st_mode )) continue;
	// Endeavor to read a single number from the file and display it
	    fin.open( filepath.c_str());
	    if (fin >> num)
		bytes_sent = write(cliSocket,"<br>\n",strlen("<br>\n"));
			if(checkBytesError(bytes_sent)){std::cout<<"Error writing to client..!"<<std::endl; return;}
	       	bytes_sent = write(cliSocket,filepath.c_str(),strlen(filepath.c_str()));
			if(checkBytesError(bytes_sent)){std::cout<<"Error writing to client..!"<<std::endl; return;}
		bytes_sent = write(cliSocket,"</br>\n",strlen("</br>\n"));
			if(checkBytesError(bytes_sent)){std::cout<<"Error writing to client..!"<<std::endl; return;}
	    fin.close();
	    }
	//close
	  closedir( dp );
}
void htmlReader(char* path,int &cliSocket){
			std::ifstream file(path);
			if(file.fail()){
				NotFoundErr(cliSocket,bytes_sent);
			}
			else{
			std::string temp = "";
			std::string headers = "HTTP/1.0 200 OK\r\nContent-type: text/html\r\n\r\n";
			bytes_sent = write(cliSocket, headers.data(), headers.length());
				if(checkBytesError(bytes_sent)){std::cout<<"Error writing to client..!"<<std::endl; return;}
			while (std::getline(file,temp)) {
				bytes_sent = write(cliSocket,temp.c_str(),strlen(temp.c_str()));
					if(checkBytesError(bytes_sent)){std::cout<<"Error writing to client..!"<<std::endl; return;}
			}
				file.close();
			}
}
std::string convertIntToString(int number){
	int length = 0;
	int copy = number;
	while(copy!=0){
	copy=copy/10;
	length++;
	}
	std::string result="";
	if(!length){ result+="0"; return result; }
	while(length!=0){
		result += char(48+number/int(pow(10,length-1)));
		number = number%int(pow(10,length-1));
		length--;
	}
return result;
}
void imageReader(char* result,std::string type,std::string subtype,int &cliSocket){
	std::ifstream file(result,std::ios::in | std::ios::binary);
		if(file.fail()){
			NotFoundErr(cliSocket,bytes_sent);
		}
		else{	
			file.seekg(0,file.end);
			ssize_t sizeF = file.tellg();
			file.seekg(0,file.beg);
			std::string headers = "HTTP/1.0 200 OK\r\nAccept-Ranges: "+convertIntToString(sizeF)+"\r\nConnection: keep-alive\r\nContent-type: "+type+"/"+subtype+"\r\n\r\n";
			bytes_sent = write(cliSocket, headers.data(), headers.length());
				if(checkBytesError(bytes_sent)){std::cout<<"Error writing to client..!"<<std::endl; return;}
			while(sizeF>0){
				file.read(incomming_data_buffer,buffersize);
				bytes_sent = write(cliSocket,incomming_data_buffer,buffersize);
					if(checkBytesError(bytes_sent)){std::cout<<"Error writing to client..!"<<std::endl; break;}
				sizeF-=bytes_sent;
				sleep(0.1);
			}
			file.close();
		}
}
   void staticFiles(int &new_sd){
	if(fullPath.substr(0,strlen("GET /"))=="GET /"){ //GET 
	        for(ssize_t i = fullPath.find("GET /") + strlen("GET /") ;fullPath[i]!=' ';i++){;
			addrcontainer+=fullPath[i]; //get the exact address
		}
		if(addrcontainer!="favicon.ico"){
				int lengthRes = strlen(addrcontainer.c_str());
				char *result = new char[lengthRes+1];
				strcpy(result,addrcontainer.c_str());
	
			if(get_ext(result)==0){ //reading gif images
				imageReader(result,"image","gif",new_sd);
			}
			if(get_ext(result)==1){	//reading html files
				if(strstr(result, "field-a=") != NULL && strstr(result, "field-b=")!=NULL){
					std::string a="",b="";
					for(ssize_t i = std::string (result).find("field-a=")+strlen("field-a=");i<strlen(result),(result[i]!='&'&& (result[i]>=char(48) && result[i]<=char(57)));i++){
						a+=result[i];
					}
					for(ssize_t i = std::string (result).find("field-a=")+strlen("field-a=") + strlen(a.c_str()) + strlen("&field-b=");i<strlen(result),(result[i]>=char(48) && result[i]<=char(57));i++){
						b+=result[i];
					}
					std::string headers = "HTTP/1.0 200 OK\r\nContent-type: text/plain\r\n\r\n";
					bytes_sent = write(new_sd, headers.data(), headers.length());
					if(strlen(a.c_str())==0 || strlen(b.c_str())==0 || result[strlen(result)-1]<char(48) || result[strlen(result)-1]>char(57)){
						bytes_sent = write(new_sd,"Incorrect input..!",strlen("Incorrect input..!"));
							if(checkBytesError(bytes_sent)){std::cout<<"Error writing to client..!"<<std::endl; return;}
					}
					else{
						int intSum = atoi(a.c_str()) + atoi(b.c_str());
						bytes_sent = write(new_sd,convertIntToString(intSum).c_str(),strlen(convertIntToString(intSum).c_str()));
							if(checkBytesError(bytes_sent)){std::cout<<"Error writing to client..!"<<std::endl; return;}
					}
				}
				else{
					htmlReader(result,new_sd);
				}
			}
			if(get_ext(result)==4){ //reading png images
				imageReader(result,"image","png",new_sd);
			}
			if( get_ext(result) == 3){ //reading jpg images
				imageReader(result,"image","jpg",new_sd);
			}
			if(get_ext(result)==2){ //reading jpeg images
				imageReader(result,"image","jpeg",new_sd);
			}
			if(get_ext(result)==5){ //reading txt files
					std::ifstream txtfile(result);
					if(txtfile.fail()){
						NotFoundErr(new_sd,bytes_sent);
					}
					else{
						std::string temp ="";
						sendResponse(new_sd,response_200);
						while (std::getline(txtfile,temp)) {
							   bytes_sent = write(new_sd,"<br>",strlen("<br>"));
								if(checkBytesError(bytes_sent)){std::cout<<"Error writing to client..!"<<std::endl; return;}
							   bytes_sent = write(new_sd,temp.c_str(),strlen(temp.c_str()));
								if(checkBytesError(bytes_sent)){std::cout<<"Error writing to client..!"<<std::endl; return;}
							   bytes_sent = write(new_sd,"</br>",strlen("</br>"));
								if(checkBytesError(bytes_sent)){std::cout<<"Error writing to client..!"<<std::endl; return;}
						   }
						bytes_sent = write(new_sd,"</body></html>\n",15);
						txtfile.close();
					}
			}
			if(get_ext(result)==6){ //reading video(mp4) files
				imageReader(result,"video","mp4",new_sd);
			}	
			if(get_ext(result)==7){ //folders 
				if(addrcontainer==""){
					htmlReader("html/home.html",new_sd);
				}
				else{
					std::ifstream fold(result);
					if(fold.fail()){
						sendResponse(new_sd,response_400);
					}else{
					   	std::string headers = "HTTP/1.0 200 OK\r\nContent-type: text/html\r\n\r\n";
						bytes_sent = write(new_sd, headers.data(), headers.length());
							if(checkBytesError(bytes_sent)){std::cout<<"Error writing to client..!"<<std::endl; return;}
						bytes_sent = write(new_sd,"<!DOCTYPE HTML>\n<html><head><title>Status 200 OK</title></head>\n",strlen("<!DOCTYPE HTML>\n<html><head><title>Status 200 OK</title></head>\n"));
							if(checkBytesError(bytes_sent)){std::cout<<"Error writing to client..!"<<std::endl; return;}
						bytes_sent = write(new_sd,"<body>\n",strlen("<body>\n"));
							if(checkBytesError(bytes_sent)){std::cout<<"Error writing to client..!"<<std::endl; return;}
						getFolderContent(result,new_sd);	
						bytes_sent = write(new_sd,"</body>\n",strlen("</body>\n"));
							if(checkBytesError(bytes_sent)){std::cout<<"Error writing to client..!"<<std::endl; return;}
						fold.close();
					}
		
			}
			}
			delete []result;
		}
	}
	if(fullPath.substr(0,strlen("POST /"))=="POST /"){ //POST
	
	}
	if(fullPath.substr(0,strlen("PUT /"))=="PUT /"){ //PUT
		
	}
	close(new_sd);
	pthread_exit(0);
}
void *handle_request(void *pcliefd) 
{
    int cliefd = *(int*)pcliefd;
    delete (int *)pcliefd;
	bytes_recieved = 0;
	totalBytesRead = 0;
	fullPath="";
	addrcontainer = "";
    do{		
	    memset(incomming_data_buffer,0,buffersize);
		sleep(0.5);
	    bytes_recieved = recv(cliefd, incomming_data_buffer,buffersize, 0);
	    totalBytesRead +=bytes_recieved;
	    if (bytes_recieved == 0){ 
		 sendResponse(cliefd, response_408);
		 std::cout<<"Bytes_recieved: 0"<<std::endl;
		 std::cout<<bytes_recieved<<"-"<<incomming_data_buffer<<std::endl;
		 close(cliefd);
		 return 0;
	    }
	    if (bytes_recieved == -1){
		 sendResponse(cliefd, response_400);
		 close(cliefd); 
		 std::cout<<"Bytes_recieved: -1"<<std::endl;
		 return 0;
	    }
	if(strstr(incomming_data_buffer,"favicon.ico")==NULL){
	    for(int i = 0;i<bytes_recieved;i++){
	         std::cout <<incomming_data_buffer[i];
	         fullPath+=incomming_data_buffer[i];
    	    }  
	}
    }while(incomming_data_buffer[bytes_recieved-4]!='\r' && incomming_data_buffer[bytes_recieved-3]!='\n' && incomming_data_buffer[bytes_recieved-2]!='\r' && incomming_data_buffer[bytes_recieved-1]!='\n');
	if(strstr(incomming_data_buffer,"favicon.ico")==NULL){
	    std::cout << totalBytesRead << " bytes recieved" << std::endl;
	}
	    staticFiles(cliefd);
	    std::cout<<"Client disconnected: "<<totalClients<<std::endl;
	    close(cliefd);
    return 0;
}

int main(int argc, const char *argv[])
{
    int status;
    struct addrinfo host_info;       
    struct addrinfo *host_info_list;
    const char* port = argv[1];
    pthread_t thread;

    memset(&host_info, 0, sizeof host_info);

    std::cout << "Setting up the structs..."  << std::endl;
    host_info.ai_family = AF_UNSPEC;     
    host_info.ai_socktype = SOCK_STREAM; 
    host_info.ai_flags = AI_PASSIVE;    

    status = getaddrinfo(NULL, port, &host_info, &host_info_list); //127.0.0.1 = NULL
    
    if (status != 0){ std::cout<<"Could not connect to port: "<<port<<std::endl; return 0; }

    std::cout << "Creating a socket..."  << std::endl;
    int socketfd ; // The socket descripter
    socketfd = socket(host_info_list->ai_family, host_info_list->ai_socktype, host_info_list->ai_protocol);
    if (socketfd == -1)  {std::cout << "socket error "<<std::endl; return 0;}

    std::cout << "Binding socket..."  << std::endl;
    
    status = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, (char*)true, sizeof(bool));
    status = bind(socketfd, host_info_list->ai_addr, host_info_list->ai_addrlen);
	if(status==-1){
		std::cout<<"Socket error "<<std::endl;
		std::cout << "Stopping server..." << std::endl;
		close(socketfd);
		freeaddrinfo(host_info_list);	
		return 0;
	}
    std::cout << "Listening for connections on port: "<<port  << std::endl;
    status = listen(socketfd, 15);
    if(status!=0){
	std::cout<<"Listen error..!"<<std::endl;
	close(socketfd);
	freeaddrinfo(host_info_list);
    }
    struct sockaddr_storage clieaddr;
    int cliefd;	
    char s[INET_ADDRSTRLEN];
    socklen_t cliesize;
    cliesize = sizeof(clieaddr);
    while(true) {
        cliefd = accept(socketfd, (struct sockaddr *)&clieaddr, &cliesize);
        if(cliefd ==-1) {
            std::cout<<"Connection failed..!"<<std::endl;
            continue;
        }
	totalClients++;
	std::cout<<"Client connected: "<<totalClients<<std::endl;
        int *pcliefd = new int;
        *pcliefd = cliefd;
        if(true) {
		sleep(0.5);
            int thrId = pthread_create(&thread, 0, handle_request, pcliefd);
		pthread_detach(thread);
	    if(thrId < 0) {
                std::cout<<"Thread err create..!"<<std::endl;
            } 
        }
        else {
            handle_request(pcliefd);
	    std::cout<<"Thread created.."<<std::endl;
        }
    }
    std::cout<<"Server is shutting down..!"<<std::endl;
    return 0;
}
