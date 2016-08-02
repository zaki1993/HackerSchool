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


//declaring some variables
const int buffersize = 1024; 
char incomming_data_buffer[buffersize];
ssize_t bytes_sent = 0;
std::string fullPath;
int new_sd; //the client socket
ssize_t bytes_recieved = 0;
ssize_t totalBytesRead = 0;
std::string addrcontainer = "";
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

const char *response_200 = "HTTP/1.1 200 OK\nContent-Type: text/html; charset=utf-8\n\n<html><body><i>Hello!</i></body></html>";
const char *response_400 = "HTTP/1.1 400 Bad Request\nContent-Type: text/html; charset=utf-8\n\n<html><body><i>Bad Request!</i></body></html>";
const char *response_404 = "HTTP/1.1 404 Not Found\nContent-Type: text/html; charset=utf-8\n\n<html><body><i>Not Found!</i></body></html>";

void NotFoundErr(int &cliSocket,ssize_t sent_bytes){ //404 NOT FOUND
	bytes_sent = write(new_sd,response_404,strlen(response_404));
}

void getFolderContent(std::string path){
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
		bytes_sent = write(new_sd,"<br>\n",strlen("<br>\n"));
	       	bytes_sent = write(new_sd,filepath.c_str(),strlen(filepath.c_str()));
		bytes_sent = write(new_sd,"</br>\n",strlen("</br>\n"));
	    fin.close();
	    }
	//close
	  closedir( dp );
}
void htmlReader(char* path){
			std::ifstream file(path);
			if(file.fail()){
				NotFoundErr(new_sd,bytes_sent);
			}
			else{
			std::string temp = "";
			std::string headers = "HTTP/1.0 200 OK\r\nContent-type: text/html\r\n\r\n";
			bytes_sent = write(new_sd, headers.data(), headers.length());
			
			while (std::getline(file,temp)) {
				   bytes_sent = write(new_sd,temp.c_str(),strlen(temp.c_str()));
			}
				file.close();
			}
}
void imageReader(char* result,std::string type,std::string subtype){
std::ifstream file(result,std::ios::in | std::ios::binary);
	if(file.fail()){
		NotFoundErr(new_sd,bytes_sent);
	}
	else{
	std::string headers = "HTTP/1.0 200 OK\r\nContent-type: "+type+"/"+subtype+"\r\n\r\n";
	bytes_sent = write(new_sd, headers.data(), headers.length());
	
	while(file.tellg()!=-1){
		file.read(incomming_data_buffer,buffersize);
		bytes_sent = write(new_sd,incomming_data_buffer,buffersize);
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
void staticFiles(){

		  for(int i = 5;fullPath[i]!=' ';i++){;
			addrcontainer+=fullPath[i]; //get the exact address
        		}
	if(addrcontainer!="favicon.ico"){
			int lengthRes = strlen(addrcontainer.c_str());
			char *result = new char[lengthRes+1];
			strcpy(result,addrcontainer.c_str());
		if(get_ext(result)==0){ //reading gif images
			imageReader(result,"image","gif");
		}
		if(get_ext(result)==1){	//reading html files
			if(strstr(result, "field-a=") != NULL && strstr(result, "field-b=")!=NULL){
				std::string a="",b="";
				for(int i = strlen("html/plus.html?field-a=");i<strlen(result),(result[i]!='&'&& (result[i]>=char(48) && result[i]<=char(57)));i++){
					a+=result[i];
				}
				for(int i = strlen("html/plus.html?field-a=") + strlen(a.c_str()) + strlen("&field-b=");i<strlen(result),(result[i]>=char(48) && result[i]<=char(57));i++){
					b+=result[i];
				}
				std::string headers = "HTTP/1.0 200 OK\r\nContent-type: text/plain\r\n\r\n";
				bytes_sent = write(new_sd, headers.data(), headers.length());
				if(strlen(a.c_str())==0 || strlen(b.c_str())==0 || result[strlen(result)-1]<char(48) || result[strlen(result)-1]>char(57)){
					bytes_sent = write(new_sd,"Incorrect input..!",strlen("Incorrect input..!"));
				}
				else{
					int intSum = atoi(a.c_str()) + atoi(b.c_str());
					bytes_sent = write(new_sd,convertIntToString(intSum).c_str(),strlen(convertIntToString(intSum).c_str()));
				}
			}
			else{
				htmlReader(result);
			}
		}
		if(get_ext(result)==4){ //reading png images
			imageReader(result,"image","png");
		}
		if( get_ext(result) == 3){ //reading jpg images
			imageReader(result,"image","jpg");
		}
		if(get_ext(result)==2){ //reading jpeg images
			imageReader(result,"image","jpeg");
		}
		if(get_ext(result)==5){ //reading txt files
				std::ifstream file(result);
				if(file.fail()){
					NotFoundErr(new_sd,bytes_sent);
				}
				else{
					std::string temp ="";
					std::string headers = "HTTP/1.0 200 OK\r\nContent-type: text/html\r\n\r\n";
					   bytes_sent = write(new_sd, headers.data(), headers.length());
					   bytes_sent = write(new_sd,"<!DOCTYPE HTML>\n<html><head><title>Status 200 OK</title></head>\n",strlen("<!DOCTYPE HTML>\n<html><head><title>Status 200 OK</title></head>\n"));
					   bytes_sent = write(new_sd,"<body>\n",strlen("<body>\n"));
				       	   bytes_sent = write(new_sd,"<p><b>200 OK.</b></p>\n",strlen("<p><b>200 OK.</b></p>\n"));
					while (std::getline(file,temp)) {
					   bytes_sent = write(new_sd,"<br>",strlen("<br>"));
					   bytes_sent = write(new_sd,temp.c_str(),strlen(temp.c_str()));
					   bytes_sent = write(new_sd,"</br>",strlen("</br>"));
					   }
					bytes_sent = write(new_sd,"</body></html>\n",15);
					file.close();
					}
				}
		if(get_ext(result)==6){ //reading video(mp4) files
			imageReader(result,"video","mp4");
		}	
		if(get_ext(result)==7){ //folders 
			if(addrcontainer==""){
				htmlReader("html/home.html");
			}
			else{
				std::ifstream fold(result);
				if(fold.fail()){
					NotFoundErr(new_sd,bytes_sent);
				}else{
				   	std::string headers = "HTTP/1.0 200 OK\r\nContent-type: text/html\r\n\r\n";
					bytes_sent = write(new_sd, headers.data(), headers.length());
					bytes_sent = write(new_sd,"<!DOCTYPE HTML>\n<html><head><title>Status 200 OK</title></head>\n",strlen("<!DOCTYPE HTML>\n<html><head><title>Status 200 OK</title></head>\n"));
					bytes_sent = write(new_sd,"<body>\n",strlen("<body>\n"));
					getFolderContent(result);	
					bytes_sent = write(new_sd,"</body>\n",strlen("</body>\n"));
					fold.close();
				}
			}
		}
		delete []result;
	}
}
int main(int argc, char *argv[])
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
    if (socketfd == -1)  std::cout << "socket error " ;

    std::cout << "Binding socket..."  << std::endl;
    
    status = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, (char*)true, sizeof(bool));
    status = bind(socketfd, host_info_list->ai_addr, host_info_list->ai_addrlen);
	if(status<0){

	std::cout<<"Socket error "<<std::endl;
	std::cout << "Stopping server..." << std::endl;
	freeaddrinfo(host_info_list);	
	return 0;
	}
    std::cout << "Listen()ing for connections..."  << std::endl;
    status =  listen(socketfd, 5);
	
	struct sockaddr_storage their_addr;
	socklen_t addr_size = sizeof(their_addr);

	while(1){
	bytes_recieved = 0;
	totalBytesRead = 0;
	fullPath="";
	addrcontainer = "";
    new_sd = accept(socketfd, (struct sockaddr *)&their_addr, &addr_size);
    if (new_sd == -1)
    {
        std::cout << "listen error" << std::endl ;
    }
    else
    {
        std::cout << "Connection accepted. Using new socketfd : "  <<  new_sd << std::endl;
    }

    std::cout<<"Buffer size: "<<sizeof(incomming_data_buffer)<<std::endl;
    std::cout << "Waiting to recieve data..."  << std::endl;
    

    do{
    bytes_recieved = recv(new_sd, incomming_data_buffer,buffersize, 0);
    totalBytesRead +=bytes_recieved;
    if (bytes_recieved == 0){ std::cout<<std::endl << "The server is waiting for your request..!" << std::endl;  close(new_sd);}
    if (bytes_recieved == -1){ std::cout<<std::endl << "recieve error!" << std::endl;  close(new_sd);}

  for(int i = 0;i<bytes_recieved;i++){
    std::cout <<incomming_data_buffer[i];
	fullPath+=incomming_data_buffer[i];
        }
}while(bytes_recieved == buffersize);
     std::cout << totalBytesRead << " bytes recieved" << std::endl;

     staticFiles();

     close(new_sd);
}
    std::cout << "Stopping server..." << std::endl;
    close(socketfd);
    freeaddrinfo(host_info_list);

return 0 ;
}
