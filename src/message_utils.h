#ifndef MESSAGE_UTILS_H
#define MESSAGE_UTILS_H

#include "socket_utils.h"
#include "cryption_utils.h"
#include <fstream>
#include <algorithm>
#include <mutex>
#include <thread>
#include <ctime>

size_t buffer_size = 65536;

class client{
    public:
        std::string name;        // Client name (optional, can be set later)
        int conn;                // Client socket file descriptor
        std::string key;         // client public key
        sockaddr_in client_addr; // client adress information
        std::string client_ip;   // client ip
	    
        // client constructor
        client(int conn = -1, const sockaddr_in& addr = {}, const std::string& ip = "") : name(""), conn(conn), key(""), client_addr(addr), client_ip(ip) {}

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

extern inline void readMessages(int server_fd, EVP_PKEY* pv_key){
    sleep(1);
    char readBuffer[buffer_size] = {0};
    // receive messages from server
    while(recv(server_fd,readBuffer,buffer_size,0) > 0){
        std::string base64_enc = readBuffer; 
        std::cout << "\nRecieved message!\n"; // << "Message contents: " << base64_enc << std::endl;
    
        // encrypted base64 -> encrypted string -> decrypted string
        std::vector<unsigned char> encrypted_binary_data = base64Decode(base64_enc);
        std::string decrypted_text = decryptWithPrivateKey(pv_key,encrypted_binary_data);
        std::cout << "decrypted message: " << decrypted_text << std::endl;
    }
    readMessages(server_fd,pv_key);
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