/* TO-DO
 *  - Send public key to server
 *  - Send Encyrpted messages to server
 *  - Get the public key from server and save it to a file
 *  - Send sender info to server
 */
#include <cstdio>
#include <iostream>
#include <ostream>
#include <string>
// #include <fstream> // for file transfers 
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#define SOCKET_PORT 8080

void handleOpenSSLError();
EVP_PKEY* loadPublicKey(const std::string& publicKeyFile);
EVP_PKEY* loadPrivateKey(const std::string& privateKeyFile);
std::vector<unsigned char> encryptWithPublicKey(EVP_PKEY* publicKey,const std::string& text);
std::string decryptWithPrivateKey(EVP_PKEY* privateKey, const std::vector<unsigned char>& encryptedText);
void connectToServer(std::string message,int port_no);


int main(int argc,char** argv){
    
    const std::string publicKeyFile = "./keys/public.pem"; 
    const std::string privateKeyFile = "./keys/private.pem";
    EVP_PKEY* publicKey = loadPublicKey(publicKeyFile);
    EVP_PKEY* privateKey = loadPrivateKey(privateKeyFile);
    if (!publicKey || !privateKey) {
        std::cerr << "Error: something happened while reading key\n";
        return 1;
    }

    std::string message = "hi guys what's up it's arz :3";
    
    std::cout << "enter your message: \n";
    std::getline(std::cin,message);



    std::vector<unsigned char> encrypted = encryptWithPublicKey(publicKey, message); 
    
    std::cout << "\nEncrypted message in hex format: \n"; // base16 
    // i will use base64 later
    // static const std::string base64letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"; 
    std::string hexEncryptedMessage;
    for (unsigned char c : encrypted) {
        char hexByte[3]; // Two characters for hex + null terminator
        snprintf(hexByte, sizeof(hexByte), "%02x", c);
        hexEncryptedMessage += hexByte;
    }

    std::cout << hexEncryptedMessage << std::endl;

    std::string decryptedMessage =  decryptWithPrivateKey(privateKey,encrypted);
    std::cout << "\nDecrpyted message: " << decryptedMessage << std::endl;

    connectToServer(hexEncryptedMessage,SOCKET_PORT);
    
    EVP_PKEY_free(publicKey);
    EVP_PKEY_free(privateKey); 
    return 0;
}




void handleOpenSSLError(){
    ERR_print_errors_fp(stderr);
    abort();
}

EVP_PKEY* loadPublicKey(const std::string& publicKeyFile){
    FILE* fp = fopen(publicKeyFile.c_str(), "r");
    if(!fp){
        std::cerr << "Error: Couldn't open public key file\n";
        fclose(fp);
        exit(1);
    }
    
    EVP_PKEY* publicKey = PEM_read_PUBKEY(fp,nullptr,nullptr,nullptr ); 
    fclose(fp);
    if(!publicKey){
        std::cerr << "Error: Couldn't read public key\n";
        handleOpenSSLError();
    }
    return publicKey;
}

EVP_PKEY* loadPrivateKey(const std::string& privateKeyFile){
    FILE* fp = fopen(privateKeyFile.c_str(), "r");
    if(!fp){
        std::cerr << "Error: Couldn't open private key file\n";
        fclose(fp); 
        exit(1);
    }
    
    EVP_PKEY* privateKey = PEM_read_PrivateKey(fp,nullptr,nullptr,nullptr ); 
    fclose(fp);

    if(!privateKey){
        std::cerr << "Error: Couldn't read private key\n";
        handleOpenSSLError();
    }
    return privateKey;
}


std::vector<unsigned char> encryptWithPublicKey(EVP_PKEY* publicKey,const std::string& text){
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(publicKey, nullptr);
    if (!ctx) handleOpenSSLError();

    if (EVP_PKEY_encrypt_init(ctx) <= 0) handleOpenSSLError();
    
    size_t outlen = 0;

    //determine buffer size for the encrypted output
    if(EVP_PKEY_encrypt(ctx, nullptr ,&outlen, reinterpret_cast<const unsigned char*>(text.c_str()),text.length()) <= 0){
        handleOpenSSLError();
    }
    std::vector<unsigned char> encrypted(outlen);  

    // encryption
    if (EVP_PKEY_encrypt(ctx, encrypted.data(), &outlen, reinterpret_cast<const unsigned char*>(text.c_str()), text.size()) <= 0){
        handleOpenSSLError();
    }   

    EVP_PKEY_CTX_free(ctx);

    return encrypted;
}

std::string decryptWithPrivateKey(EVP_PKEY* privateKey, const std::vector<unsigned char>& encryptedText) {
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(privateKey, nullptr);
    if (!ctx) handleOpenSSLError();

    if (EVP_PKEY_decrypt_init(ctx) <= 0) handleOpenSSLError();

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

void connectToServer(std::string message,int port_no = SOCKET_PORT){
    int clientSocket = socket(AF_INET ,SOCK_STREAM ,0);
    if(clientSocket == -1){
        std::cerr << "Socket Creation Failed\n";
        exit(1); 
    }
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port_no);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    int connection = connect(clientSocket ,reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress));
    if(connection == -1){
        std::cerr << "Connection failed\n";
        exit(1);
    }
    const char* msg = message.c_str();
    send(clientSocket,msg,strlen(msg),0);
    std::cout <<  "Connected!\n";
}