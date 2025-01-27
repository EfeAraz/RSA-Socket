#ifndef MESSAGE_UTILS_H
#define MESSAGE_UTILS_H

#include "socket_utils.h"
#include "cryption_utils.h"
#include <cstddef>
#include <fstream>
#include <algorithm>
#include <mutex>
#include <openssl/evp.h>
#include <string>
#include <thread>
#include <ctime>

constexpr size_t BUFFER_SIZE = 65536;
std::mutex clientsMutex;

class client{
    public:
        std::string name;        // Client name (optional, can be set later)
        int conn;                // Client socket file descriptor
        std::string key;         // client public key
        sockaddr_in client_addr; // client adress information
        std::string client_ip;   // client ip
	    client* peer = nullptr;
        int peer_conn;
        bool keyACK = false;
        bool peerKeyACK = false;
        // client constructor
        client(int conn = -1, const sockaddr_in& addr = {}, const std::string& ip = "") 
        : name(""), conn(conn), key(""), client_addr(addr), client_ip(ip) {}
};


inline void setPeer(client* client1,client* client2){
    std::lock_guard<std::mutex> lock(clientsMutex);
    client1->peer = client2;
    client2->peer = client1;
    std::cout << "Peer relationship established: " << client1->conn << " <-> " << client2->conn << "\n";
    std::cout << client1->conn << " peer: " << client1->peer->conn << " ," << client2->conn << " peer: " << client2->peer->conn << std::endl;
}


inline void logMessage(std::string logMessage,std::string& logLocation){
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

inline int sendMessage(int clientSocket,std::string message){
    const char* msg = message.c_str();
    if(send(clientSocket,msg,message.length(),0) >= 0){
        std::cout <<  "\t\tMessage Sent!\n";
        return 1;
    }
    std::cerr << "\tCouldn't send message\n";
    return -1;
}

inline void readMessages(int client_fd, EVP_PKEY* pv_key,EVP_PKEY*& pb_key,std::string& tempKeyFile){
    while(true){
        try{
            char readBuffer[BUFFER_SIZE] = {0};
            // receive messages from server
            if(recv(client_fd, readBuffer, BUFFER_SIZE, 0) <= 0) {
                std::cerr << "Connection closed\n";
                pb_key = nullptr;
                break;
            }
            
            std::string msg_recv = readBuffer; 
            if(!msg_recv.empty()){
                if (msg_recv.rfind("----",0) == 0){
                    std::cout << "\nRECIEVED A KEY!!!\n";
                    // print recieved key
                    //std::cout << msg_recv << std::endl; 
                    std::fstream p_bFile(tempKeyFile,std::fstream::out);
                    if(!p_bFile){
                        std::cerr << "Unable to open other public key file\n";
                    }
                    else{
                        p_bFile << msg_recv;
                        std::cout << "Saved key into " << tempKeyFile << "\n\n";
                        p_bFile.close();
                        pb_key = loadPublicKey(tempKeyFile);
                    }
                }
                else{
                    if (msg_recv.rfind("==", msg_recv.size() - 2) == (msg_recv.size() - 2)){                
                        // std::cout << "\nRecieved message! "; // << "Message contents: " << msg_recv << std::endl;
                        // encrypted base64 -> encrypted string -> decrypted string
                        std::vector<unsigned char> encrypted_binary_data = base64Decode(msg_recv);
                        std::string decrypted_text = decryptWithPrivateKey(pv_key,encrypted_binary_data);
                        std::cout << "\nother client: " << decrypted_text << std::endl;
                    }
                    else{
                        std::cerr << "\nMESSAGE FROM SERVER: " << msg_recv << std::endl;
                    }
                    // std::cout << "Enter your message (type 'exit' to quit): ";
                }
            }
        }
        catch(const std::exception& e){
            std::cerr << "An error occured while reading" << e.what() << "\n";
        }
    }
}

/* // sending files does not work 
inline int sendFile(int client_sock, std::string filePath,EVP_PKEY* pb_key){
    std::ifstream file(filePath,std::ifstream::binary);
    if(!file){
        std::cerr << "Couldnt open file: " << filePath << std::endl;
        return -1;
    }
    const size_t chunk_size = 230;
    char chunk_buffer[chunk_size];
    unsigned char encrypted_Buffer[EVP_PKEY_size(pb_key)];
    
    // read and send file in chunks
    std::cout << "sending file\n";
    sendMessage(client_sock,"SENDING FILE\n");

    while(!file.eof()){
        file.read(chunk_buffer,chunk_size);
        size_t bytesRead = file.gcount();

        if(bytesRead > 0){
            std::vector<unsigned char> encryptedMessage = encryptWithPublicKey(pb_key, chunk_buffer);  
            std::string base64MessageEncrypted = base64Encode(encryptedMessage);
            if (base64MessageEncrypted.empty()) {
                std::cerr << "Error: Something happened while encrypting base64 data so it's empty!\n";
                file.close();
                return -1;
            }
            std::cout << "\nEncrypted message in base64 format:\n" << base64MessageEncrypted << "\n";
            sendMessage(client_sock,base64MessageEncrypted);
        }
    }
    file.close();
    std::cout << "File sent!";
    return 1;
}
*/

inline void sendPublicKeyToServer(EVP_PKEY* publicKey,int clientSocket){
    std::string pemKey = publicKeyToPEMString(publicKey);
    if(sendMessage(clientSocket,pemKey) < 0){
        std::cerr << "Couldn't Send Key to Server\n";
        return;
    }
    std::cout << "Key Sent\n";
}

inline std::vector<std::string> readFileContent(std::string &fileAddress){
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

inline int recvPublicKeyFromServer(std::string &otherPublicKeyFileLocation,int clientSocket){
    char *recv_buf = new char[BUFFER_SIZE]; // buffer  2^16 because why not  
    std::cout << "Reading other public key from server\n";
    if(recv(clientSocket,recv_buf,BUFFER_SIZE,0) > 0){
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