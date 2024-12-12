#ifndef CLIENT_H
#define CLIENT_H
#include "message.h"
#include <fstream>
class client{
    private:
        std::string private_key;
        int userID;
    public:
        std::string public_key;
        std::string name;
        std::string IP_address;

        client(){
            public_key = readFile("keys/public.key");
            private_key = readFile("keys/private.key");
            // IP_address = system("ipconfig > ip.txt"); // Çalışmıyor
        } 

        std::string readFile(std::string file_name){ 
            std::string file_contents;
            std::ifstream file(file_name);
            
                if ( file.is_open() ){
                    std::string file_line;
                    while (std::getline(file, file_line)) {
                        file_contents += file_line + "\n"; 
                        }
                }
                else {
                    std::cerr << "Failed to open public key file.\n"; 
                }
            return file_contents;
        }   
        
    void printKeys(){
        std::cout << this->private_key << "\n";
        std::cout << this->public_key << "\n";
    }


    message encrypt(message msg, std::string ){
        message encrypted_msg;


        return encrypted_msg;
    }

    message decrypt(message msg){
        message decrypted_msg;
        // Decrypt using private key
        // this->private_key;

        return decrypted_msg;
    }
    void ConnectToTheServer(){
        // Socket Connection
        // ...
        
        message firstMessage;
        /* First Message:
            - Username
            - userID
            - Public Key - 
        */
        firstMessage.content = this->name + "\n" 
            + std::to_string(this->userID) + "\n"
            + this->public_key;

        // Send First Message to Server 

        // Keep The Connection Alive
    }


};

#endif // CLIENT_H