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
const int buffersize = 1024;
char incomming_data_buffer[buffersize];
ssize_t bytes_sent = 0;
std::string fullPath;
ssize_t bytes_recieved = 0;
ssize_t totalBytesRead = 0;
std::string addrcontainer = "";
int closeSockets = 0;

/* RESPONSES */
    const char *response_200 = "HTTP/1.0 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n\r\n<html><body><i></i>200 OK</body></html>";
    const char *response_400 = "HTTP/1.0 400 Bad Request\r\nContent-Type: text/html; charset=utf-8\r\n\r\n<html><body><i>Bad Request!</i></body></html>";
    const char *response_404 = "HTTP/1.0 404 Not Found\r\nContent-Type: text/html; charset=utf-8\r\n\r\n<html><body><i>Not Found!</i></body></html>";
    const char *response_408 = "HTTP/1.0 408 Request Timeout\r\nContent-Type: text/html; charset=utf-8\r\n\r\n<html><body><i>Request Timeout</i></body></html>";
    const char *response_download = "HTTP/1.0 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n\r\n<html><body><i><h1>DOWNLOADED</h1></i></body></html>";
/* RESPONSES */

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
    return notfound;
}
bool checkBytesError(ssize_t &num){
	if(num==-1){
		return true;
	}
	return false;
}
void sendResponse(int &socket,const char *response){
	bytes_sent = write(socket,response,strlen(response));
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
void htmlReader(std::string result,int &cliSocket){
	std::ifstream htm;
	if(strstr(result.c_str(),"&download")!=NULL && result.find("&download")==(strlen(result.c_str())-strlen("&download")) && (result.find(".html")+1==result.find("&download")-strlen("html"))){
		htm.open(result.substr(0,result.find("&download")).c_str());
		if(htm.fail()){
			sendResponse(cliSocket,response_404);
			return;
		}
		else{
			std::string headers = "HTTP/1.0 200 OK\r\n Content-Description: File Transfer\r\nConnection: keep-alive\r\nContent-type: application/force-download \r\n\r\n";
			bytes_sent = write(cliSocket, headers.c_str(),strlen(headers.c_str()));
		}
	}
	else{	
		htm.open(result.c_str());
		if(htm.fail()){
			sendResponse(cliSocket,response_404);
			return;
		}
		else{
			std::string headers = "HTTP/1.0 200 OK\r\nContent-type: text/html\r\n\r\n";
			bytes_sent = write(cliSocket, headers.c_str(), strlen(headers.c_str()));
		}
	}
	std::string temp = "";
	if(checkBytesError(bytes_sent)){std::cout<<"Error writing to client..!"<<std::endl; return;}
	while (std::getline(htm,temp)) {
		bytes_sent = write(cliSocket,temp.c_str(),strlen(temp.c_str()));
		if(checkBytesError(bytes_sent)){std::cout<<"Error writing to client..!"<<std::endl; return;}
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
void imageReader(std::string result,std::string type,std::string subtype,int &cliSocket){
	std::ifstream file;
	if(strstr(result.c_str(),"&download")!=NULL && result.find("&download")==(strlen(result.c_str())-strlen("&download")) && (result.find("."+subtype)+1==result.find("&download")-strlen(subtype.c_str()))){
			file.open(result.substr(0,result.find("&download")).c_str(),std::ios::out | std::ios::binary);
			if(file.fail()){
				sendResponse(cliSocket,response_404);
				return;
			}
			else{
				std::string headers = "HTTP/1.0 200 OK\r\n Content-Description: File Transfer\r\nConnection: keep-alive\r\nContent-type: application/force-download \r\n\r\n";
				bytes_sent = write(cliSocket, headers.data(), headers.length());
			}
		}
	else{
		    file.open(result.c_str(),std::ios::out | std::ios::binary);
			if(file.fail()){
				sendResponse(cliSocket,response_404);
				return;
			}
			else{	
				std::string headers = "HTTP/1.0 200 OK\r\nConnection: keep-alive\r\nContent-type: "+type+"/"+subtype+"\r\n\r\n";
				bytes_sent = write(cliSocket, headers.data(), headers.length());
			}

		}
	file.seekg(0,file.end);
	ssize_t sizeF = file.tellg();
	file.seekg(0,file.beg);
	if(checkBytesError(bytes_sent)){std::cout<<"Error writing to client..!"<<std::endl; return;}
	while(sizeF>0){
		file.read(incomming_data_buffer,buffersize);
		bytes_sent = write(cliSocket,incomming_data_buffer,buffersize);
		if(checkBytesError(bytes_sent)){std::cout<<"Error writing to client..!"<<std::endl; break;}
		sizeF-=bytes_sent;
	}
	file.close();
}
void readTxt(std::string name,int &cliSocket){
	std::ifstream txtfile;
	if(strstr(name.c_str(),"&download")!=NULL && name.find("&download")==(strlen(name.c_str())-strlen("&download")) && (name.find(".txt")+1==name.find("&download")-strlen("txt"))){
		txtfile.open(name.substr(0,name.find("&download")).c_str());
		if(txtfile.fail()){
			sendResponse(cliSocket,response_404);
			return;
		}
		else{
			std::string headers = "HTTP/1.0 200 OK\r\n Content-Description: File Transfer\r\nConnection: keep-alive\r\nContent-type: application/force-download \r\n\r\n";
			bytes_sent = write(cliSocket, headers.data(), headers.length());
		}
	}
	else{
		txtfile.open(name.c_str());
		if(txtfile.fail()){
			sendResponse(cliSocket,response_404);
			return;
		}
		else{
			std::string headers = "HTTP/1.0 200 OK\r\nConnection: keep-alive\r\nContent-type: text/plain\r\n\r\n";
			bytes_sent = write(cliSocket, headers.data(), headers.length());
		}
	}
	std::string temp ="";
	while (std::getline(txtfile,temp)) {
		   bytes_sent = write(cliSocket,temp.c_str(),strlen(temp.c_str()));
			if(checkBytesError(bytes_sent)){std::cout<<"Error writing to client..!"<<std::endl; return;}
		   bytes_sent = write(cliSocket,"\r\n",strlen("\r\n"));
		}
	 txtfile.close();
}
void GETMethod(int &new_sd,std::string addrcontainer){
		if(get_ext(addrcontainer)==0){ //reading gif images
			imageReader(addrcontainer,"image","gif",new_sd);
		}
		else if(get_ext(addrcontainer)==1){	//reading html files
			if(strstr(addrcontainer.c_str(), "field-a=") != NULL && strstr(addrcontainer.c_str(), "field-b=")!=NULL){
				std::string a="",b="";
				for(ssize_t i = addrcontainer.find("field-a=")+strlen("field-a=");i<strlen(addrcontainer.c_str()),(addrcontainer.c_str()[i]!='&'&& (addrcontainer.c_str()[i]>=char(48) && addrcontainer.c_str()[i]<=char(57)));i++){
					a+=addrcontainer.c_str()[i];
				}
				for(ssize_t i = addrcontainer.find("field-a=")+strlen("field-a=") + strlen(a.c_str()) + strlen("&field-b=");i<strlen(addrcontainer.c_str()),(addrcontainer.c_str()[i]>=char(48) && addrcontainer.c_str()[i]<=char(57));i++){
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
				htmlReader(addrcontainer,new_sd);
			}
		}
		else if(get_ext(addrcontainer)==4){ //reading png images
			imageReader(addrcontainer,"image","png",new_sd);
		}
		else if( get_ext(addrcontainer) == 3){ //reading jpg images
			imageReader(addrcontainer,"image","jpg",new_sd);
		}
		else if(get_ext(addrcontainer)==2){ //reading jpeg images
			imageReader(addrcontainer,"image","jpeg",new_sd);
		}
		else if(get_ext(addrcontainer)==5){ //reading txt files
				readTxt(addrcontainer,new_sd);
		}
		else if(get_ext(addrcontainer)==6){ //reading video(mp4) files
			imageReader(addrcontainer,"video","mp4",new_sd);
		}	
		else { //folders 
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
void POSTMethod(int &new_sd,std::string addrcontainer){
//comming soon;
	std::cout<<addrcontainer<<std::endl;
	if(strstr(addrcontainer.c_str(),"uploads/upload.html")!=NULL){	//reading html files
				htmlReader("uploads/upload.html",new_sd); //uploaded.html ***
	}
}
   void staticFiles(int &new_sd){
   		addrcontainer = "";
   		usleep(2500);
   		if(fullPath.substr(0,strlen("GET /"))=="GET /"){ //GET 
	        for(ssize_t i = fullPath.find("GET /") + strlen("GET /") ;fullPath[i]!=' ';i++){
				addrcontainer+=fullPath[i]; //get the exact address
			}
			GETMethod(new_sd,addrcontainer);
		}
		if(fullPath.substr(0,strlen("POST /"))=="POST /"){ //POST
			 for(ssize_t i = fullPath.find("POST /") + strlen("POST /") ;fullPath[i]!=' ';i++){
				addrcontainer+=fullPath[i]; //get the exact address
			}
			POSTMethod(new_sd,addrcontainer);
		}
}
bool handle_request(int &cliefd) 
{	
	totalBytesRead = 0;
	fullPath="";
	int contentLength = 0;
	std::ofstream requestFile("request.txt",std::ios::out | std::ios::binary);
	std::string fileName = "";
	std::string fileExtension = "";
	bool checkPost = false;
    do{
   		usleep(2500);
			 bytes_recieved = recv(cliefd, incomming_data_buffer,buffersize, 0);
			 if (bytes_recieved == 0){ 
			 		if(sleep(5) && (bytes_recieved = recv(cliefd, incomming_data_buffer,buffersize, 0))==0){
			 			return false;
			 		}
			    }
			    else if (bytes_recieved == -1){
					 std::cout<<"Bytes_recieved: -1"<<std::endl;	
					 exit(0);
			    }
			    else if(bytes_recieved>0){
			    	totalBytesRead+=bytes_recieved;
					if(strstr(incomming_data_buffer,"favicon.ico")==NULL){
							std::cout<<incomming_data_buffer<<std::endl;
					}					
					if(strstr(incomming_data_buffer,"favicon.ico")==NULL){
					    for(int i = 0;i<bytes_recieved;i++){
					         requestFile<<incomming_data_buffer[i];
					         fullPath+=incomming_data_buffer[i];
				        }  
				    }
				    if(strstr(incomming_data_buffer,"POST /")!=NULL){
				    	checkPost = true;
				    }
				    if(strstr(incomming_data_buffer,"POST /")!=NULL){
					    if(strstr(incomming_data_buffer,"Content-Length: ")!=NULL){
					    	std::string temporary = std::string (incomming_data_buffer);
					    	contentLength = atoi((temporary.substr(temporary.find("Content-Length: ")+strlen("Content-Length: "),temporary.find("Cache-Control: "))).c_str());
					    }
					    if(strstr(incomming_data_buffer,"filename=\"")!=NULL && strstr(incomming_data_buffer,"Content-Type")!=NULL){
					    	std::string nameTemp = std::string(incomming_data_buffer);
					    	nameTemp = nameTemp.substr(nameTemp.find("filename=\"")+strlen("filename=\""));
					    	fileName = nameTemp.substr(nameTemp.find("\""));
					    }
					    if(strstr(incomming_data_buffer,"Content-Type: ")!=NULL){
					    	std::string tempExtension = std::string(incomming_data_buffer);
					    	tempExtension = tempExtension.substr(tempExtension.find("Content-Type: "));
					    	fileExtension = tempExtension.substr(tempExtension.find("/"));
					    }
					}
			    }		   	    
	}while(strstr(incomming_data_buffer,"\r\n\r\n")==NULL || (checkPost && totalBytesRead<=contentLength));	
	   
	    staticFiles(cliefd);
	    requestFile.close();
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
    
    status = setsockopt(socketfd, SOL_SOCKET, SO_KEEPALIVE, (char*)true, sizeof(bool));
    status = bind(socketfd, host_info_list->ai_addr, host_info_list->ai_addrlen);
	if(status==-1){
		std::cout<<"Socket error "<<std::endl;
		std::cout << "Stopping server..." << std::endl;
		if(close(socketfd)<0){
			std::cout<<"Socket close error: SERVER"<<std::endl;
		}
		freeaddrinfo(host_info_list);	
		return 0;
	}
    std::cout << "Listening for connections on port: "<<port  << std::endl;
    status = listen(socketfd, 100);
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
    while((cliefd = accept(socketfd, (struct sockaddr *)&clieaddr, &cliesize))) {
    	static int counter = 0;
    	counter++;
    	if(counter%5000==0){
    		usleep(50000);
    	}
        if(cliefd ==-1) {
            std::cout<<"Connection failed..!"<<std::endl;
            continue;
        }
        	int pid;
	        while((pid = fork())<0){
	        	usleep(1000);
	        	std::cout<<"no child process"<<std::endl;
	        }
	        if(pid==0){
	        	usleep(500);
	        	if(close(cliefd)<0){
	        		std::cout<<"Socket close error: CLIENT"<<std::endl;
	        	}
	        }
	        else{
	        	usleep(500);
	        	if(close(socketfd)<0){
	        		std::cout<<"Socket close error: SERVER"<<std::endl;
	        	}
        		int result = handle_request(cliefd);
        		if(!result){
        			std::cout<<"0 bytes_recieved"<<std::endl;
        		}
        		exit(0);
	        }
    }
    freeaddrinfo(host_info_list);
    return 0;
}
