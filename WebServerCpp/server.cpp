#include <iostream>
#include <string>
#include <cstring>      
#include <sys/socket.h> 
#include <netdb.h>      
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h> 
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>

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

ssize_t bytes_sent = 0;
std::string fullPath;
int new_sd; //the client socket

void NotFoundErr(int &cliSocket,ssize_t sent_bytes){ //404 NOT FOUND function template
				std::cout<<"File not found..!"<<std::endl;
				bytes_sent = write(new_sd,"<!DOCTYPE HTML>\n<html><head><title>Error 404 Not Found!!</title></head>\n",strlen("<!DOCTYPE HTML>\n<html><head><title>Error 404 Not Found!!</title></head>\n"));
	    		        bytes_sent = write(new_sd,"<body>\n",strlen("<body>\n"));
	     			bytes_sent = write(new_sd,"<p><b>404 FILE NOT FOUND.</b></p>\n",strlen("<p><b>404 FILE NOT FOUND.</b></p>\n"));
	     			bytes_sent = write(new_sd,"</body></html>\n",15);
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
	  closedir( dp );
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
    socketfd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
                      host_info_list->ai_protocol);
    if (socketfd == -1)  std::cout << "socket error " ;

    std::cout << "Binding socket..."  << std::endl;
    
    int yes = 1;
    status = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    status = bind(socketfd, host_info_list->ai_addr, host_info_list->ai_addrlen);
	if(status<0){

	std::cout<<"Socket error "<<std::endl;
	 std::cout << "Stopping server..." << std::endl;
	freeaddrinfo(host_info_list);	
	return 0;
	}
    std::cout << "Listen()ing for connections..."  << std::endl;
    status =  listen(socketfd, 5);

	while(1){
	fullPath="";
	std::string addrcontainer = "";
    struct sockaddr_storage their_addr;
    socklen_t addr_size = sizeof(their_addr);
    new_sd = accept(socketfd, (struct sockaddr *)&their_addr, &addr_size);
    if (new_sd == -1)
    {
        std::cout << "listen error" << std::endl ;
    }
    else
    {
        std::cout << "Connection accepted. Using new socketfd : "  <<  new_sd << std::endl;
    }

    ssize_t bytes_recieved = 0;
    ssize_t totalBytesRead = 0;
    const int buffersize = 1024; 
    char incomming_data_buffer[buffersize];
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

		  for(int i = 5;fullPath[i]!=' ';i++){;
			addrcontainer+=fullPath[i]; //get the exact address
        	}
	
	if(addrcontainer!="favicon.ico"){
		int lengthRes = strlen(addrcontainer.c_str());
		char *result = new char[lengthRes+1];
			strcpy(result,addrcontainer.c_str());
	if(get_ext(result)==0){ //reading gif images
		std::ifstream file(result,std::ios::in | std::ios::binary);
			if(file.fail()){
				NotFoundErr(new_sd,bytes_sent);
			}
			else{
			std::string headers = "HTTP/1.0 200 OK\r\nContent-type: image/gif\r\n\r\n";
			bytes_sent = write(new_sd, headers.data(), headers.length());
			
			while(file.tellg()!=-1){
				file.read(incomming_data_buffer,buffersize);
				bytes_sent = write(new_sd,incomming_data_buffer,buffersize);
			}
				file.close();
			}
	}
	if(get_ext(result)==1){	//reading html files
		
	}
	if(get_ext(result)==4){ //reading png images
		std::ifstream file(result,std::ios::in | std::ios::binary);
			if(file.fail()){
				NotFoundErr(new_sd,bytes_sent);
			}
			else{
			std::string headers = "HTTP/1.0 200 OK\r\nContent-type: image/png\r\n\r\n";
			bytes_sent = write(new_sd, headers.data(), headers.length());
			
			while(file.tellg()!=-1){
				file.read(incomming_data_buffer,buffersize);
				bytes_sent = write(new_sd,incomming_data_buffer,buffersize);
			}
				file.close();
			}
	}
	if( get_ext(result) == 3){ //reading jpg images
		std::ifstream file(result,std::ios::in | std::ios::binary );
			if(file.fail()){
				NotFoundErr(new_sd,bytes_sent);
			}
			else{
			std::string headers = "HTTP/1.0 200 OK\r\nContent-type: image/jpg\r\n\r\n";
			bytes_sent = write(new_sd, headers.data(), headers.length());
			
			while(file.tellg()!=-1){
				file.read(incomming_data_buffer,buffersize);
				bytes_sent = write(new_sd,incomming_data_buffer,buffersize);
			}
				file.close();
			}
	}
	if(get_ext(result)==2){ //reading jpeg images
		std::ifstream file(result,std::ios::in | std::ios::binary);
			if(file.fail()){
				NotFoundErr(new_sd,bytes_sent);
			}
			else{
			std::string headers = "HTTP/1.0 200 OK\r\nContent-type: image/jpeg\r\n\r\n";
			bytes_sent = write(new_sd, headers.data(), headers.length());
			
			while(file.tellg()!=-1){
				file.read(incomming_data_buffer,buffersize);
				bytes_sent = write(new_sd,incomming_data_buffer,buffersize);
			}
				file.close();
			}
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
	
		std::ifstream file(result,std::ios::in | std::ios::binary);
			if(file.fail()){
				NotFoundErr(new_sd,bytes_sent);
			}
			else{
			std::string headers = "HTTP/1.0 200 OK\r\nContent-type: video/mp4\r\n\r\n";
			bytes_sent = write(new_sd, headers.data(), headers.length());
			
			while(file.tellg()!=-1){
				file.read(incomming_data_buffer,buffersize);
				bytes_sent = write(new_sd,incomming_data_buffer,buffersize);
			}
				file.close();
			}
			
	}	
	if(get_ext(result)==7){ //folders 
		if(addrcontainer==""){
			std::string headers = "HTTP/1.0 200 OK\r\nContent-type: text/html\r\n\r\n";
			bytes_sent = write(new_sd, headers.data(), headers.length());
			bytes_sent = write(new_sd,"<!DOCTYPE HTML>\n<html><head><title>Home page</title></head>\n",strlen("<!DOCTYPE HTML>\n<html><head><title>Home page</title></head>\n"));
			bytes_sent = write(new_sd,"<body>\n",strlen("<body>\n"));
			bytes_sent = write(new_sd,"</body>\n",strlen("</body>\n"));
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
			}
		}
	}
	delete []result;
	}
     close(new_sd);
}


    std::cout << "Stopping server..." << std::endl;
    freeaddrinfo(host_info_list);
    close(socketfd);
return 0 ;
}
