#ifndef MESSAGE_UTILS_H
#define MESSAGE_UTILS_H

#include "socket_utils.h"
#include "cryption_utils.h"
#include <fstream>

class client{
    public:
        std::string name;
        int conn;
        std::string key;
    private:
};

extern inline void logMessage(std::string logMessage,std::string& logLocation){
    std::ofstream f(logLocation, std::ofstream::app); // append
    if(!f){
        std::cerr << "log file is not found/can't be opened\n";
    }
    else{
        f << logMessage << std::endl;
        std::cout << logMessage << std::endl;
        f.close();
    }
}

extern inline int sendMessage(int clientSocket,std::string message){
    const char* msg = message.c_str();
    if(send(clientSocket,msg,strlen(msg),0) >= 0){
        std::cout <<  "Message Sent!\n";
        return 1;
    }
    std::cerr << "Couldn't send message\n";
    return -1;
}

extern inline void sendPublicKeyToServer(EVP_PKEY* publicKey,int clientSocket){
    std::string pemKey = publicKeyToPEMString(publicKey);
    if(sendMessage(clientSocket,pemKey) < 0){
        std::cerr << "Couldn't Send Key to Server\n";
        return;
    }
    std::cout << "Key Sent\n";
}

extern inline std::vector<std::string> readFileContent(std::string &fileAddress){
    std::vector<std::string> output;
    std::cout << "Reading file at: " << fileAddress << "\n" ;
    std::ifstream fileStream(fileAddress);
    std::string line;
    if(!fileStream){
        std::cout << "failed to open the file";
        output.push_back("failed to open the file");
    }
    else{
        std::cout << "File contents:\n";
        while(std::getline(fileStream,line)){
            output.push_back(line);
            std::cout << line << std::endl;
        }
    }
    return output;
}

extern inline int recvPublicKeyFromServer(std::string &otherPublicKeyFileLocation,int clientSocket){
    size_t buffer_size = 65536;
    char *recv_buf = new char[buffer_size]; // buffer  2^16 because why not  
    std::cout << "Reading other public key from server\n";
    if(recv(clientSocket,recv_buf,buffer_size,0) > 0){
        //read keys
        std::string otherPublicKey = recv_buf;
        free(recv_buf);
        // save keys into file
        std::fstream otherPublicKeyFile;
        otherPublicKeyFile.open(otherPublicKeyFileLocation,std::fstream::out);
        if(!otherPublicKeyFile){
            std::cerr << "Unable to open other public key file\n";
            return -1;
        }
        otherPublicKeyFile << otherPublicKey << std::endl;
        std::cout << "Saved key into " << otherPublicKeyFileLocation << "\n";
        otherPublicKeyFile.close();
        return 1;
    }
    else{
        std::cout << "Failed to write into other public key file\n";
        return -1;
    }
}

#endif