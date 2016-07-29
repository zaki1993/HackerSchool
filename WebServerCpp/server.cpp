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

typedef enum {mp4, html, jpeg, jpg, plain,notfound} ext;

ext get_ext(char *file) {
    if (strstr(file, ".mp4") != NULL)
        return mp4;
    if (strstr(file, ".html") != NULL)
        return html;
    if (strstr(file, ".jpeg") != NULL)
        return jpeg;
    if (strstr(file, ".jpg") != NULL)
        return jpg;
    if (strstr(file, ".txt") != NULL)
        return plain;
    else{
	return notfound;
	}
}

ssize_t bytes_sent = 0;
std::string fullPath;
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
    int new_sd;
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
        }
}while(bytes_recieved == buffersize);
     std::cout << totalBytesRead << " bytes recieved" << std::endl;

		  for(int i = 5;i<bytes_recieved,incomming_data_buffer[i]!=' ';i++){;
			addrcontainer+=incomming_data_buffer[i]; //get the exact address
        	}
	
	if(addrcontainer!="favicon.ico"){
	std::cout<<"Container: "<<addrcontainer<<std::endl;
		int lengthRes = strlen(addrcontainer.c_str());
		char *result = new char[lengthRes+1];
			strcpy(result,addrcontainer.c_str());
	if(get_ext(result)==1){
		
	}
	if(get_ext(result)==1){	
		
	}
	if(get_ext(result)==2 || get_ext(result) == 3){
		std::ifstream file(result,std::ios::out | std::ios::binary);
			if(file.fail()){
				std::cout<<"File not found..!"<<std::endl;
				bytes_sent = write(new_sd,"<!DOCTYPE HTML>\n<html><head><title>Error 404 (Not Found)!!</title></head>\n",strlen("<!DOCTYPE HTML>\n<html><head><title>Error 404 (Not Found)!!</title></head>\n"));
	    		        bytes_sent = write(new_sd,"<body>\n",strlen("<body>\n"));
	     			bytes_sent = write(new_sd,"<p><b>404 IMAGE FILE NOT FOUND.</b></p>\n",strlen("<p><b>404 JPEG FILE NOT FOUND.</b></p>\n"));
	     			bytes_sent = write(new_sd,"</body></html>\n",15);
			}
			else{
				while(file.good()){
					file.read(incomming_data_buffer,1);
					bytes_sent = write(new_sd,incomming_data_buffer,1);
				}
			}
	}
	if(get_ext(result)==4){
			std::ifstream file(result);
			if(file.fail()){
				std::cout<<"File not found..!"<<std::endl;
				bytes_sent = write(new_sd,"<!DOCTYPE HTML>\n<html><head><title>Error 404 (Not Found)!!</title></head>\n",strlen("<!DOCTYPE HTML>\n<html><head><title>Error 404 (Not Found)!!</title></head>\n"));
	    		        bytes_sent = write(new_sd,"<body>\n",strlen("<body>\n"));
	     			bytes_sent = write(new_sd,"<p><b>404 FILE NOT FOUND.</b></p>\n",strlen("<p><b>404 FILE NOT FOUND.</b></p>\n"));
	     			bytes_sent = write(new_sd,"</body></html>\n",15);
			}
			else{
				std::cout<<"Txt file opened..!"<<std::endl;
				std::string temp ="";
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
	if(get_ext(result)==5){
	
				std::cout<<"Unknown file type..!"<<std::endl;
				bytes_sent = write(new_sd,"<!DOCTYPE HTML>\n<html><head><title>Error 404 (Not Found)!!</title></head>\n",strlen("<!DOCTYPE HTML>\n<html><head><title>Error 404 (Not Found)!!</title></head>\n"));
	    		        bytes_sent = write(new_sd,"<body>\n",strlen("<body>\n"));
	     			bytes_sent = write(new_sd,"<p><b>404 FILE NOT FOUND.</b></p>\n",strlen("<p><b>404 FILE NOT FOUND.</b></p>\n"));
				bytes_sent = write(new_sd,"<p><h1>Unknown file type</h1></p>\n",strlen("<p><h1>Unknown file type</h1></p>\n"));
	     			bytes_sent = write(new_sd,"</body></html>\n",15);

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
