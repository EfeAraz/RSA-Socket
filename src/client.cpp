#include <fstream>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include "message.h"
#include "client.h"

// TO-DO
// -Fix decrypt-encrypt file location
// -Send public key to server
// -Send Encyrpted messages to server 

class client;

int main(int argc,char** argv){
    message msg;
    client user;
    user.name = "arz";
    if(argc <= 1){ 
        std::cout << "Enter Your Message: " << "\n";
        std::getline(std::cin,msg.content);
    }
    else{
        std::string tempmsg;
        for(int i = 1; i < argc; i++){
            std::cout << "Message "<< i << ": " << argv[i] << "\n";
            tempmsg += argv[i];
            tempmsg += ' ';
    }
        msg.content = tempmsg;
    }
    std::cout << user.name << ": " << msg.content << "\n";
    
    return 0;
}


