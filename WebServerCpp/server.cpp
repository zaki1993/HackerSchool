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
int closeSockets = 0;
    const char *response_200 = "HTTP/1.0 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n\r\n<html><body><i></i></body></html>";
    const char *response_400 = "HTTP/1.0 400 Bad Request\r\nContent-Type: text/html; charset=utf-8\r\n\r\n<html><body><i>Bad Request!</i></body></html>";
    const char *response_404 = "HTTP/1.0 404 Not Found\r\nContent-Type: text/html; charset=utf-8\r\n\r\n<html><body><i>Not Found!</i></body></html>";
    const char *response_408 = "HTTP/1.0 408 Request Timeout\r\nContent-Type: text/html; charset=utf-8\r\n\r\n<html><body><i>Request Timeout</i></body></html>";

typedef enum {gif, html, jpeg, jpg, png, plain,mp4, notfound} ext; //all the extensions that the web server recognises

//function that finds the extension of the file
ext get_ext(std::string file1) {
    if (strstr(file1.c_str(), ".gif") != NULL)
        return gif;
    if (strstr(file1.c_str(), ".html") != NULL)
        return html;
    if (strstr(file1.c_str(), ".jpeg") != NULL)
        return jpeg;
    if (strstr(file1.c_str(), ".jpg") != NULL)
        return jpg;
    if (strstr(file1.c_str(), ".png") != NULL)
        return png;
    if (strstr(file1.c_str(), ".txt") != NULL)
        return plain;
    if(strstr(file1.c_str(), ".mp4") != NULL)
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
void htmlReader(std::string path,int &cliSocket){
			std::ifstream htm(path.c_str()	);
			if(htm.fail()){
				NotFoundErr(cliSocket,bytes_sent);
				return;
			}
			else{
			std::string temp = "";
			std::string headers = "HTTP/1.0 200 OK\r\nContent-type: text/html\r\n\r\n";
			bytes_sent = write(cliSocket, headers.data(), headers.length());
				if(checkBytesError(bytes_sent)){std::cout<<"Error writing to client..!"<<std::endl; return;}
			while (std::getline(htm,temp)) {
				bytes_sent = write(cliSocket,temp.c_str(),strlen(temp.c_str()));
					if(checkBytesError(bytes_sent)){std::cout<<"Error writing to client..!"<<std::endl; return;}
			}
			}htm.close();
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
void imageReader(std::string result,std::string type,std::string subtype,int &cliSocket){
	std::ifstream file(result.c_str(),std::ios::in | std::ios::binary);
		if(file.fail()){
			NotFoundErr(cliSocket,bytes_sent);
			return;
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
			}
		}file.close();
}
   void staticFiles(int &new_sd){
   		usleep(2500);
   		if(fullPath.substr(0,strlen("GET /"))=="GET /"){ //GET 
	        for(ssize_t i = fullPath.find("GET /") + strlen("GET /") ;fullPath[i]!=' ';i++){
			addrcontainer+=fullPath[i]; //get the exact address
		}
		if(addrcontainer!="favicon.ico"){
				
			if(get_ext(addrcontainer.c_str())==0){ //reading gif images
				imageReader(addrcontainer,"image","gif",new_sd);
			}
			if(get_ext(addrcontainer.c_str())==1){	//reading html files
				if(strstr(addrcontainer.c_str(), "field-a=") != NULL && strstr(addrcontainer.c_str(), "field-b=")!=NULL){
					std::string a="",b="";
					for(ssize_t i = std::string (addrcontainer.c_str()).find("field-a=")+strlen("field-a=");i<strlen(addrcontainer.c_str()),(addrcontainer.c_str()[i]!='&'&& (addrcontainer.c_str()[i]>=char(48) && addrcontainer.c_str()[i]<=char(57)));i++){
						a+=addrcontainer.c_str()[i];
					}
					for(ssize_t i = std::string (addrcontainer.c_str()).find("field-a=")+strlen("field-a=") + strlen(a.c_str()) + strlen("&field-b=");i<strlen(addrcontainer.c_str()),(addrcontainer.c_str()[i]>=char(48) && addrcontainer.c_str()[i]<=char(57));i++){
						b+=addrcontainer.c_str()[i];
					}
					std::string headers = "HTTP/1.0 200 OK\r\nContent-type: text/plain\r\n\r\n";
					bytes_sent = write(new_sd, headers.data(), headers.length());
					if(strlen(a.c_str())==0 || strlen(b.c_str())==0 || addrcontainer.c_str()[strlen(addrcontainer.c_str())-1]<char(48) || addrcontainer.c_str()[strlen(addrcontainer.c_str())-1]>char(57)){
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
					htmlReader(addrcontainer.c_str(),new_sd);
				}
			}
			if(get_ext(addrcontainer.c_str())==4){ //reading png images
				imageReader(addrcontainer,"image","png",new_sd);
			}
			if( get_ext(addrcontainer.c_str()) == 3){ //reading jpg images
				imageReader(addrcontainer,"image","jpg",new_sd);
			}
			if(get_ext(addrcontainer.c_str())==2){ //reading jpeg images
				imageReader(addrcontainer,"image","jpeg",new_sd);
			}
			if(get_ext(addrcontainer.c_str())==5){ //reading txt files
					std::ifstream txtfile(addrcontainer.c_str());
					if(txtfile.fail()){
						NotFoundErr(new_sd,bytes_sent);
						return;
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
					}txtfile.close();
			}
			if(get_ext(addrcontainer.c_str())==6){ //reading video(mp4) files
				imageReader(addrcontainer,"video","mp4",new_sd);
			}	
			if(get_ext(addrcontainer.c_str())==7){ //folders 
				if(addrcontainer==""){
					htmlReader("html/home.html",new_sd);
				}
				else{
					std::ifstream fold(addrcontainer.c_str());
					if(fold.fail()){
						sendResponse(new_sd,response_400);
						return;
					}else{
					   	std::string headers = "HTTP/1.0 200 OK\r\nContent-type: text/html\r\n\r\n";
						bytes_sent = write(new_sd, headers.data(), headers.length());
							if(checkBytesError(bytes_sent)){std::cout<<"Error writing to client..!"<<std::endl; return;}
						bytes_sent = write(new_sd,"<!DOCTYPE HTML>\n<html><head><title>Status 200 OK</title></head>\n",strlen("<!DOCTYPE HTML>\n<html><head><title>Status 200 OK</title></head>\n"));
							if(checkBytesError(bytes_sent)){std::cout<<"Error writing to client..!"<<std::endl; return;}
						bytes_sent = write(new_sd,"<body>\n",strlen("<body>\n"));
							if(checkBytesError(bytes_sent)){std::cout<<"Error writing to client..!"<<std::endl; return;}
						getFolderContent(addrcontainer.c_str(),new_sd);	
						bytes_sent = write(new_sd,"</body>\n",strlen("</body>\n"));
							if(checkBytesError(bytes_sent)){std::cout<<"Error writing to client..!"<<std::endl; return;}
					}
					fold.close();
			}
			}
		}
	}
	if(fullPath.substr(0,strlen("POST /"))=="POST /"){ //POST
	
	}
	if(fullPath.substr(0,strlen("PUT /"))=="PUT /"){ //PUT
		
	}
}
bool handle_request(int &cliefd) 
{
	totalBytesRead = 0;
	fullPath="";
	addrcontainer = "";

 	if(cliefd>0){
	   do{
	   		usleep(2500);
				 bytes_recieved = recv(cliefd, incomming_data_buffer,buffersize, 0);
				 if (bytes_recieved == 0){ 
				 		return false;
				    }
				    else if (bytes_recieved == -1){
					 std::cout<<"Bytes_recieved: -1"<<std::endl;	
					 exit(0);
				    }
				    else if(bytes_recieved>0){
				    	totalBytesRead+=bytes_recieved;
						if(strstr(incomming_data_buffer,"favicon.ico")==NULL){
								std::cout<<incomming_data_buffer<<std::endl;
						    for(int i = 0;i<bytes_recieved;i++){
						         fullPath+=incomming_data_buffer[i];
					    	    }  
						}
				    }
		} while(incomming_data_buffer[bytes_recieved-2]!='\n' && incomming_data_buffer[bytes_recieved-1]!='\n');
	}
	else{
		exit(0);
	}
	if(strstr(incomming_data_buffer,"favicon.ico")==NULL){
	    std::cout << totalBytesRead << " bytes recieved" << std::endl;
	}	

	    staticFiles(cliefd);
	    return true;
}

int main(int argc, const char *argv[])
{
    int status;
    struct addrinfo host_info;       
    struct addrinfo *host_info_list;
    const char* port = argv[1];
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
    if (socketfd == -1) {std::cout << "socket error "<<std::endl; return 0;}

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
    status = listen(socketfd, 50);
    if(status!=0){
		std::cout<<"Listen error..!"<<std::endl;
		close(socketfd);
		freeaddrinfo(host_info_list);
		return 0;
    }
    struct sockaddr_storage clieaddr;
    int cliefd;	
    char s[INET_ADDRSTRLEN];
    socklen_t cliesize;
    cliesize = sizeof(clieaddr);
    while(1) {
    	static int counter = 0;
    	counter++;
    	if(counter%5000==0){
    		usleep(50000);
    	}
        cliefd = accept(socketfd, (struct sockaddr *)&clieaddr, &cliesize);
        if(cliefd ==-1) {
            std::cout<<"Connection failed..!"<<std::endl;
            continue;
        }
            if(fork()==0){
            	if((closeSockets = close(socketfd))<0){
            		std::cout<<"Error closing server socket..!"<<std::endl;
            	}
        			if(!handle_request(cliefd)){ //run this function
        				exit(0);
        			}
        			else{
		            	if((closeSockets = close(cliefd))<0){
		            		std::cout<<"Error closing client socket..!"<<std::endl;
		            	}
		            	exit(0);
           			 }	
            }
            else{
	            if((closeSockets = close(cliefd))<0){
	            		std::cout<<"Error closing client socket..!"<<std::endl;
	           	}
	           	continue;
	           }
    }
    close(socketfd);
    freeaddrinfo(host_info_list);
    return 0;
}


