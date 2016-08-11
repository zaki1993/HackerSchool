#include <cstdio>
#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <fstream>

template <typename TP>
std::string num2str( TP const& value ){
    std::stringstream sin;
    sin << value;
    return sin.str();
}

int Execute_Command( const std::string&  command, std::string& output,const std::string& mode = "r")
{
    // Create the stringstream
    std::stringstream sout;

    // Run Popen
    FILE *in;
    char buff[512];

    // Test output
    if(!(in = popen(command.c_str(), mode.c_str()))){
        return 1;
    }

    // Parse output
    while(fgets(buff, sizeof(buff), in)!=NULL){
        sout << buff;
    }

    // Close
    int exit_code = pclose(in);

    // set output
    output = sout.str();

    // Return exit code
    return exit_code;
}

bool Ping(const std::string& address, const int& max_attempts, std::string& details )
{
    // Format a command string
    std::string command = "ping -c " + num2str(max_attempts) + " " + address + " 2>&1";
    std::string output;

    // Execute the ping command
    int code = Execute_Command( command, details );

    return (code == 0);
}


int main( int argc, char* argv[] )
{
    // Parse input
    if( argc < 2 ){
        std::cerr << "usage: " << argv[0] << " <address> <max-attempts = 3>" << std::endl;
        return 1;
    }

    // Get the address
    std::string NamePlusHost = argv[1];
    std::string hostName = NamePlusHost.substr(0,NamePlusHost.find(":"));
    std::string hostAddress = NamePlusHost.substr(NamePlusHost.find(":")+1);
    std::string packetsLost;
    // Get the max attempts
    int max_attempts = 1;
    if( argc > 2 ){
        max_attempts = atoi(argv[2]);
    }
    if( max_attempts < 1 ){
        std::cerr << "max-attempts must be > 0" << std::endl;
        return 1;
    }

    // Execute the command
    std::string details;
    bool result = Ping( hostAddress, max_attempts, details );
    if(strstr(details.c_str(),"unknown host")!=NULL){
    	packetsLost = "100";
    }
    else{
	    std::string packet = details.substr(details.find("received, ")+strlen("received, "));
	    packetsLost = packet.substr(0, packet.find('%'));//get lost packets
	}
    std::ofstream file("packets.json",std::ios::out);
    if(!file.fail()){
    	file<<"{"<<std::endl;
    	file<<"  \"data\": ["<<std::endl;
    	file<<"  {"<<std::endl;
    	file<<"   \"{#NAME}\": ";
    	file<<"\""<<hostName<<"\", "<<std::endl;
    	file<<"   \"{#IPADDRESS}\": ";
    	file<<"\""<<hostAddress<<"\", "<<std::endl;
    	file<<"   \"{#VALUE}\": ";
    	file<<"\""<<packetsLost<<"\""<<std::endl;
    	file<<"  }"<<std::endl;
    	file<<" ]"<<std::endl;
    	file<<"}"<<std::endl;
    	file.close();
    }
    return 0;
}