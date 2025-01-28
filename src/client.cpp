#include "socket_utils.h"
#include "cryption_utils.h"
#include "message_utils.h"
#include <iostream>

// default file locations, you can change it if you want to
const std::string privateKeyFileLocation = "./keys/private.pem";    // public key file
const std::string publicKeyFileLocation = "./keys/public.pem";      // private key file
const std::string logLocation = "/tmp/RSA-socket.log";                    // log file
std::string otherPublicKeyFileLocation;                             // other public key file, later it will be created as /tmp/"username"+peer.pem, change it if you want to

int main(int argc,char** argv){
    // get ip & port from command line arguments
    // probably not safe
    if(argc!=4){
        std::cerr << "Correct Usage: " << argv[0] << " ip_adress port_no username\n";
        return 1;
    }
    // probably not safe 
    // [command], -server address-, -port- , -username/temp key location-
    std::string serverIP = argv[1]; 
    int port = atoi(argv[2]);   
    std::string username = argv[3];   
    otherPublicKeyFileLocation = "/tmp/"+ username +"peer.pem";         // for saving the other key later
    
    
    logMessage("started logging at location: " + logLocation ,logLocation);
    
    // create socket file descriptor after connecting to server 
    int clientSocket = connectToServer(serverIP,port);
    if(clientSocket == -1){         // check if socket fd is created properly
        std::cerr << "Error while setting up client socket\n";
        return 1;
    }

    // load keys
    EVP_PKEY* privateKey = loadPrivateKey(privateKeyFileLocation);
    EVP_PKEY* publicKey = loadPublicKey(publicKeyFileLocation);
    if (!publicKey || !privateKey) {    // upon fail loading close the socket, free the keys
        std::cerr << "Error: Failed while reading public/private key\n";
        close(clientSocket);
        EVP_PKEY_free(privateKey);
        EVP_PKEY_free(publicKey); 
        return 1;
    }

    // send public key to server
    sendPublicKeyToServer(publicKey,clientSocket);
    //recieve public key from server
    //read the public key which we'll use to 
    EVP_PKEY* other_pb_key = nullptr;
    
    // create thread for reading messages 
    std::thread(readMessages,clientSocket,privateKey,std::ref(other_pb_key),std::ref(otherPublicKeyFileLocation)).detach();

    
    // run till you get a SIGINT or get "exit" as your message
    while (true) {     
        try{
            // get message from user
            std::string message = "";
            // std::cout << "Enter your message (type 'exit' to quit): ";
            std::getline(std::cin, message);

            if (message == "exit") {
                break;
            }
            if(other_pb_key != nullptr){ // if other public key exists, encrypt the message and send it to the server
                // std::cout << "Key exists!\n";
                std::vector<unsigned char> encryptedMessage = encryptWithPublicKey(other_pb_key, message);  // encrypt using the other key
                std::string base64MessageEncrypted = base64Encode(encryptedMessage);                                // encode the encrypted message to base64 format             
                // std::cout << "\nEncrypted message in base64 format:\n" << base64MessageEncrypted << "\n";
                sendMessage(clientSocket,base64MessageEncrypted);
            }
            else{ // if other public key does not exists send 
                std::cout << "Pair key DOES NOT exist! Sending message using local public key\n";
                std::vector<unsigned char> encryptedMessage = encryptWithPublicKey(publicKey, message);
                std::string base64MessageEncrypted = base64Encode(encryptedMessage);
                // std::cout << "\nEncrypted message in base64 format:\n" << base64MessageEncrypted << "\n";
                sendMessage(clientSocket,base64MessageEncrypted);    
            }
        }
        catch(const std::exception& e){
            std::cerr << "An error occured " << e.what() << "\n";
        }
    }

    // after exit free the keys and close the socket fd
    close(clientSocket);
    EVP_PKEY_free(publicKey);
    EVP_PKEY_free(privateKey);
    std::cout << "Cleanup complete!\n";
    return 0;
}