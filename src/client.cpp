/* TO-DO
 *  - Get the public key from server and save it to a file
 *  - Send sender info to server
 */
#include <cstdio>
#include <iostream>
#include <ostream>
#include <string>
// #include <fstream> // for file transfers 
#include <vector>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>
 
#define SOCKET_PORT 8080

// Function declarations
void handleOpenSSLError();
EVP_PKEY* loadPublicKey(const std::string& publicKeyFile);
EVP_PKEY* loadPrivateKey(const std::string& privateKeyFile);
std::vector<unsigned char> encryptWithPublicKey(EVP_PKEY* publicKey,const std::string& text);
std::string decryptWithPrivateKey(EVP_PKEY* privateKey, const std::vector<unsigned char>& encryptedText);
int connectToServer(int port_no);
int sendMessage(int ClientSocket, std::string message, int port_no);
std::string base64Encode(const std::vector<unsigned char>& input);
std::vector<unsigned char> base64Decode(const std::string& base64Text);
void sendPublicKeyToServer(int clientSocket,EVP_PKEY* publicKey,int port_no);

int main(int argc,char** argv){
    
    const std::string publicKeyFile = "./keys/public.pem"; 
    const std::string privateKeyFile = "./keys/private.pem";
    EVP_PKEY* publicKey = loadPublicKey(publicKeyFile);
    EVP_PKEY* privateKey = loadPrivateKey(privateKeyFile);
    if (!publicKey || !privateKey) {
        std::cerr << "Error: something happened while reading key\n";
        return 1;
    }
    int clientSocket = connectToServer(SOCKET_PORT); 
    sendPublicKeyToServer(clientSocket,publicKey,SOCKET_PORT);
    // get other person's public key from server 
    std::string message = "hi guys :3";

    while (true) {
            std::string message;
            std::cout << "Enter your message (type 'exit' to quit): ";
            std::getline(std::cin, message);

            if (message == "exit") {
                break;
            }
    
    std::vector<unsigned char> encrypted = encryptWithPublicKey(publicKey, message);  
    std::string base64EncodedMessage = base64Encode(encrypted);
    
    /*
    std::string hexEncryptedMessage;
    for (unsigned char c : encrypted) {
        char hexByte[3]; // Two characters for hex + null terminator
        snprintf(hexByte, sizeof(hexByte), "%02x", c);
        hexEncryptedMessage += hexByte;
    }
    std::cout << "\nEncrypted message in hex format: \n"; // base16 
    std::cout << hexEncryptedMessage << std::endl;
    */
    /*
    std::string decryptedMessage =  decryptWithPrivateKey(privateKey,encrypted);
    std::cout << "\nDecrpyted message: " << decryptedMessage << std::endl;
    */

    std::cout << "\nEncrypted message in base64 format: \n"; // base16 
    std::cout << base64EncodedMessage << std::endl;
    

    
    std::vector<unsigned char> base64DecodededMessage = base64Decode(base64EncodedMessage);
    std::string decryptedMessage =  decryptWithPrivateKey(privateKey,base64DecodededMessage);
    std::cout << "\nDecrpyted message: " << decryptedMessage << std::endl;

    sendMessage(clientSocket,base64EncodedMessage,SOCKET_PORT);
    sleep(1);
    }
    close(clientSocket);
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

int connectToServer(int port_no = SOCKET_PORT){
    int clientSocket = socket(AF_INET ,SOCK_STREAM ,0);
    if(clientSocket == -1){
        std::cerr << "Socket Creation Failed\n";
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port_no);
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    int connection = connect(clientSocket ,reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress));
    if(connection == -1){
        std::cerr << "Connection failed\n";
        return 1;
    }
    std::cout << "Connected to server\n";
    return clientSocket;
}

int sendMessage(int clientSocket,std::string message,int port_no = SOCKET_PORT){
    const char* msg = message.c_str();
    if(send(clientSocket,msg,strlen(msg),0) >= 0){
        std::cout <<  "Message Sent!\n";
        return 1;
    }
    std::cerr << "Couldn't send message\n";
    return -1;
    
}


std::string base64Encode(const std::vector<unsigned char>& input) {
    BIO* bio = BIO_new(BIO_s_mem());
    BIO* b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    // Disable newline breaks in Base64 encoding
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

    BIO_write(bio, input.data(), input.size());
    BIO_flush(bio);

    char* encoded;
    long encodedLen = BIO_get_mem_data(bio, &encoded);
    std::string result(encoded, encodedLen);

    BIO_free_all(bio);
    return result;
}


std::vector<unsigned char> base64Decode(const std::string& base64Text) {
    BIO* bio = BIO_new_mem_buf(base64Text.data(), base64Text.size());
    BIO* b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    // Disable newline breaks in Base64 decoding
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

    std::vector<unsigned char> decoded(base64Text.size());
    int decodedLength = BIO_read(bio, decoded.data(), base64Text.size());
    if (decodedLength <= 0) {
        BIO_free_all(bio);
        std::cerr << "Failed to decode Base64";
    }

    decoded.resize(decodedLength);
    BIO_free_all(bio);
    return decoded;
}

std::string publicKeyToPEM(EVP_PKEY* publicKey) {
    BIO* bio = BIO_new(BIO_s_mem());
    if (!PEM_write_bio_PUBKEY(bio, publicKey)) {
        BIO_free(bio);
        std::cerr << "Failed to convert public key to PEM format\n";
    }

    char* pemData;
    long pemLength = BIO_get_mem_data(bio, &pemData);
    std::string pemString(pemData, pemLength);

    BIO_free(bio);
    return pemString;
}

void sendPublicKeyToServer(int clientSocket,EVP_PKEY* publicKey,int port_no = SOCKET_PORT){
    std::string pemKey = publicKeyToPEM(publicKey);
    if(sendMessage(clientSocket,pemKey,port_no) < 0){
        std::cerr << "Couldn't Send Key to Server\n";
        return;
    }
    std::cout << "Key Sent\n";
}