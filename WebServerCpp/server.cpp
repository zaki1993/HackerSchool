#include <iostream>
#include <cstring>      
#include <sys/socket.h> 
#include <netdb.h>      
int main()
{

    int status;
    struct addrinfo host_info;       
    struct addrinfo *host_info_list; 
    const char* port = "1263";
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


    std::cout << "Waiting to recieve data..."  << std::endl;
    ssize_t bytes_recieved;
    char incomming_data_buffer[1000];
    bytes_recieved = recv(new_sd, incomming_data_buffer,1000, 0);

    if (bytes_recieved == 0) std::cout << "host shut down." << std::endl ;
    if (bytes_recieved == -1)std::cout << "recieve error!" << std::endl ;
    std::cout << bytes_recieved << " bytes recieved :" << std::endl ;
    incomming_data_buffer[bytes_recieved] = '\0';
    std::cout << incomming_data_buffer << std::endl;


    std::cout << "send()ing back a message..."  << std::endl;
    const char *msg = "thank you.";
    int len;
    ssize_t bytes_sent;
    len = strlen(msg);
    bytes_sent = send(new_sd, msg, len, 0);

    std::cout << "Stopping server..." << std::endl;
    freeaddrinfo(host_info_list);
    //close(new_sd);
    //close(socketfd);

return 0 ;


}
