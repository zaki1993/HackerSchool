#include <iostream>
#include <cstring>      
#include <sys/socket.h> 
#include <netdb.h>      
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h> 


#define STAT_200 " 200 OK\r\n"
#define STAT_404 " 404 Not Found\r\n"
#define STAT_501 " 501 Not Implemented\r\n"

#define F_DIR "Content-Type: text/directory\r\n"
#define F_GIF "Content-Type: image/gif\r\n"
#define F_HTML "Content-Type: text/html\r\n"
#define F_JPEG "Content-Type: image/jpeg\r\n"
#define F_JPG "Content-Type: image/jpg\r\n"
#define F_TXT "Content-Type: text/plain\r\n"

typedef enum {cgi, gif, html, jpeg, jpg, plain} ext;

ext get_ext(char *file) {
    if (strstr(file, ".cgi") != NULL)
        return cgi;
    if (strstr(file, ".gif") != NULL)
        return gif;
    if (strstr(file, ".html") != NULL)
        return html;
    if (strstr(file, ".jpeg") != NULL)
        return jpeg;
    if (strstr(file, ".jpg") != NULL)
        return jpg;
    if (strstr(file, ".txt") != NULL)
        return plain;
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
    int new_sd;
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


    ssize_t bytes_recieved;
    const int buffersize = 1024; 
    char incomming_data_buffer[buffersize];
	std::cout<<"Buffer size: "<<sizeof(incomming_data_buffer)<<std::endl;
	    std::cout << "Waiting to recieve data..."  << std::endl;
    bytes_recieved = recv(new_sd, incomming_data_buffer,buffersize, 0);
	
    if (bytes_recieved == 0) std::cout << "host shut down." << std::endl ;
    if (bytes_recieved == -1)std::cout << "recieve error!" << std::endl ;
    std::cout << bytes_recieved << " bytes recieved :" << std::endl ;
if(bytes_recieved != buffersize){
    incomming_data_buffer[bytes_recieved] = '\0';
    std::cout << "response: "<<incomming_data_buffer << std::endl;
 }
else{	
// std::cout << "response: "<<incomming_data_buffer << std::endl;

}
    

     ssize_t bytes_sent;
     bytes_sent = write(new_sd,"HTTP/1.0 200 OK\n",16); // Write back to client. to bound socket from client. 
     bytes_sent = write(new_sd,"Connection: Keep-alive\n",23);
     bytes_sent= write(new_sd,"Connection: close\n",18);
     bytes_sent = write(new_sd,"Content-type: text/html; charset=UTF-8\n",39);
     bytes_sent = write(new_sd,"\n",1);
     bytes_sent = write(new_sd,"<!DOCTYPE HTML>\n<html><head><title>Hello World!</title></head>\n",strlen("<!DOCTYPE HTML>\n<html><head><title>Hello World!</title></head>\n"));
     bytes_sent = write(new_sd,"<body><h1>Hello World!</h1>\n",strlen("<body><h1>Hello World!</h1>\n"));
     bytes_sent = write(new_sd,"<div><h1>ZdraZdrastiZdZdraZdrastiZdrastiZdraZdrastiZdrastiZdraZdrastiZdrastirasti</h1></div>\n",strlen("<div><h1>ZdraZdrastiZdZdraZdrastiZdrastiZdraZdrastiZdrastiZdraZdrastiZdrastirasti</h1></div>\n"));
     bytes_sent = write(new_sd,"<div><h1>ZdraZdrastiZdZdraZdrastiZdrastiZdraZdrastiZdrastiZdraZdrastiZdrastirasti</h1></div>\n",strlen("<div><h1>ZdraZdrastiZdZdraZdrastiZdrastiZdraZdrastiZdrastiZdraZdrastiZdrastirasti</h1></div>\n"));
     bytes_sent = write(new_sd,"<div><h1>ZdraZdrastiZdZdraZdrastiZdrastiZdraZdrastiZdrastiZdraZdrastiZdrastirasti</h1></div>\n",strlen("<div><h1>ZdraZdrastiZdZdraZdrastiZdrastiZdraZdrastiZdrastiZdraZdrastiZdrastirasti</h1></div>\n"));
     bytes_sent = write(new_sd,"<div><h1>ZdraZdrastiZdZdraZdrastiZdrastiZdraZdrastiZdrastiZdraZdrastiZdrastirasti</h1></div>\n",strlen("<div><h1>ZdraZdrastiZdZdraZdrastiZdrastiZdraZdrastiZdrastiZdraZdrastiZdrastirasti</h1></div>\n"));
     bytes_sent = write(new_sd,"</body></html>\n",15);
close(new_sd);
 
	}
    std::cout << "Stopping server..." << std::endl;
    freeaddrinfo(host_info_list);
   close(socketfd);
return 0 ;


}
