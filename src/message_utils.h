#ifndef MESSAGE_UTILS_H
#define MESSAGE_UTILS_H

#include "socket_utils.h"
#include "cryption_utils.h"
#include <fstream>

class client{
    public:
        std::string name;
        int conn;
    private:
};

extern inline std::vector<unsigned char> encryptWithPublicKey(EVP_PKEY* publicKey,const std::string& text){
    // read key content  
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(publicKey, nullptr);
    //if can't allocate throw error 
    if (!ctx)
         handleOpenSSLError(); 
    // if can't initialize encryption throw error
    if (EVP_PKEY_encrypt_init(ctx) <= 0)
         handleOpenSSLError();

    size_t outlen = 0; // buffer size
    //determine buffer size for the decyrpted output, if failed to determine throw error
    if(EVP_PKEY_encrypt(ctx, nullptr ,&outlen, reinterpret_cast<const unsigned char*>(text.c_str()),text.length()) <= 0){
        handleOpenSSLError();
    }
    std::vector<unsigned char> encrypted(outlen);  

    // decryption
    if (EVP_PKEY_encrypt(ctx, encrypted.data(), &outlen, reinterpret_cast<const unsigned char*>(text.c_str()), text.size()) <= 0){
        handleOpenSSLError();
    }   
    // free memory
    EVP_PKEY_CTX_free(ctx);
    return encrypted;
}


extern inline std::string decryptWithPrivateKey(EVP_PKEY* privateKey, const std::vector<unsigned char>& encryptedText) {
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(privateKey, nullptr);  // key context 
    // if can't read the key throw error
    if (!ctx) 
        handleOpenSSLError();
    // if can't initialize decryption throw error
    if (EVP_PKEY_decrypt_init(ctx) <= 0)
        handleOpenSSLError();

    size_t outlen = 0;
    // determine buffer size for the decrypted output
    if (EVP_PKEY_decrypt(ctx, nullptr, &outlen, encryptedText.data(), encryptedText.size()) <= 0)
        handleOpenSSLError();

    std::vector<unsigned char> decrypted(outlen);

    // decryption
    if (EVP_PKEY_decrypt(ctx, decrypted.data(), &outlen, encryptedText.data(), encryptedText.size()) <= 0)
        handleOpenSSLError();

    EVP_PKEY_CTX_free(ctx); 

    return std::string(decrypted.begin(), decrypted.end());
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
    std::cout << "File contents:\n";
    while(std::getline(fileStream,line)){
        output.push_back(line);
        std::cout << line<< std::endl;
    }
    return output;
}

extern inline void recvPublicKeyFromServer(std::string &otherPublicKeyFileLocation,int clientSocket){
    size_t buffer_size = 65536;
    char *recv_buf = new char[buffer_size]; // buffer  2^16 because why not  

    if(recv(clientSocket,recv_buf,buffer_size,0) > 0){
        //read keys
        std::string otherPublicKey = recv_buf;
        std::cout << "Read key from server\n";
        // save keys into file
        std::fstream otherPublicKeyFile(otherPublicKeyFileLocation,std::fstream::in | std::fstream::out); // read & write into file
        if(!otherPublicKeyFile){
            std::cerr << "Unable to open other public key file\n";
        }
        else{
            otherPublicKeyFile << otherPublicKey << std::endl;
            std::cout << "Saved key into " << otherPublicKeyFileLocation << "\n";
            otherPublicKeyFile.close();
        }        
    }
    else{
        std::cerr << "Failed to recieve the key from server\n";
    }
    free(recv_buf);
}

extern inline void logMessage(std::string& logMessage,std::string& logLocation){
    std::fstream f;
    f.open(logLocation,std::ios::app);
    if(!f){
        std::cerr << "log file is not found";
    }
    else{
        f << logMessage;
        std::cout << "log successfull \n";
        f.close();
    }
}

#endif