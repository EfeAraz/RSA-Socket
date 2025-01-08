#include "socket_utils.h"
#include "cryption_utils.h"
#include "message_utils.h"

const std::string privateKeyFileLocation = "./keys/private.pem";
const std::string publicKeyFileLocation = "./keys/public.pem";  
std::string logger = "/tmp/RSA-socket.log";

int main(int argc,char** argv){
    // get ip & port from command line argumentss
    // probably not safe
    if(argc!=4){
        std::cerr << "Correct Usage: " << argv[0] << " ip_adress port_no username\n";
        return 1;
    }
    // probably not safe at all but whatever
    std::string serverIP = argv[1];
    int port = atoi(argv[2]);
    std::string username = argv[3];
    std::string otherPublicKeyFileLocation = "/tmp/"+ username +"peer.pem"; 


    logMessage("started logging at location: " + logger ,logger);
    
    // create file descriptor for connection
    int clientSocket = connectToServer(serverIP,port);
    if(clientSocket == -1){
        std::cerr << "Error while setting up client socket\n";
        return 1;
    }

    // load keys
    EVP_PKEY* privateKey = loadPrivateKey(privateKeyFileLocation);
    EVP_PKEY* publicKey = loadPublicKey(publicKeyFileLocation);
    if (!publicKey || !privateKey) {
        std::cerr << "Error: Failed while reading public/private key\n";
        close(clientSocket);
        EVP_PKEY_free(publicKey);
        EVP_PKEY_free(privateKey); 
        return 1;
    }
    // send public key to server, then free memory
    sendPublicKeyToServer(publicKey,clientSocket);
    //recieve public key from server
    //read the public key which we'll use to  

    std::thread(readMessages,clientSocket,privateKey,publicKey,std::ref(otherPublicKeyFileLocation)).detach();

    // send encrypted messages to server
    while (true) {     
        try{
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
        catch(const std::exception& e){
            std::cerr << "An error occured " << e.what() << "\n";
        }
    }

    // after exit free/close everything
    close(clientSocket);
    EVP_PKEY_free(publicKey);
    EVP_PKEY_free(privateKey);
    std::cout << "Cleanup complete!\n";
    return 0;
}