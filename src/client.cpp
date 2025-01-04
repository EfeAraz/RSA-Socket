#include <ctime>
#include "socket_utils.h"
#include "cryption_utils.h"
#include "message_utils.h"

int port = 8080;
std::string serverIP = "192.168.1.1"; 

const std::string privateKeyFile = "./keys/private.pem";
const std::string publicKeyFile = "./keys/public.pem"; 
const std::string otherPublicKeyFile = "./keys/public.pem"; // get this key from server

int main(int argc,char** argv){
    if(argc!=3){
        std::cerr << "Correct Usage: " << argv[0] << " ip_adress port_no\n";
        return 1;
    }   
    
    // read inputs
    // probably not safe
    serverIP = argv[1];
    port = atoi(argv[2]);

    // connect to the server
    int clientSocket = connectToServer(serverIP,port);
    if(clientSocket == -1){
        std::cerr << "Error while setting up client socket\n";
        return 1;
    }

    // load keys
    EVP_PKEY* privateKey = loadPrivateKey(privateKeyFile);
    EVP_PKEY* publicKey = loadPublicKey(publicKeyFile);
    if (!publicKey || !privateKey) {
        std::cerr << "Error: Failed while reading key\n";
        close(clientSocket);
        EVP_PKEY_free(publicKey);
        EVP_PKEY_free(privateKey); 
        return 1;
    }


    sendPublicKeyToServer(clientSocket, publicKey, port);
    
    //  recieve other public key from server 
    try{
        while (true) {     
            std::string message = "";
            std::cout << "Enter your message (type 'exit' to quit): ";
            std::getline(std::cin, message);

            if (message == "exit") {
                break;
            }
            std::vector<unsigned char> encryptedMessage = encryptWithPublicKey(publicKey, message);  
            std::string base64MessageEncrypted = base64Encode(encryptedMessage);
            std::cout << "\nEncrypted message in base64 format:\n" << base64MessageEncrypted << "\n";
            sendMessage(clientSocket,base64MessageEncrypted);
            sleep(1);
        }
    }
    catch(const std::exception& e){
        std::cerr << "An error occured " << e.what() << "\n";
    }
    close(clientSocket);
    EVP_PKEY_free(publicKey);
    EVP_PKEY_free(privateKey);
    std::cout << "Cleanup complete!\n";
    return 0;
}