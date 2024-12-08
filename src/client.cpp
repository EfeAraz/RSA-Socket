#include <fstream>
#include <iostream>
#include <string>
#include <sys/socket.h>
// TO-DO
// -Fix decrypt-encrypt file location
// -Send public key to server
// -Send Encyrpted messages to server 


class message{
    private:
        int message_id;
    public:
        int author_id;
        std::string author_name;
        std::string content;

};

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




int main(int argc,char** argv){
    int port_no = 8079;
    message msg;
    client user;
    user.name = "arz";
    if(argc <= 1){ 
        std::cout << "Enter Your Message: " << "\n";
        std::getline(std::cin,msg.content);
    }
    else{
        std::string tempmsg;
        for(int i = 1; i < argc; i++){
            /*if((argv[i] == "-u") && (i + 1 < argc)){
                user.name = argv[i+1];
                i++;
            }
            else
            */{
                std::cout << "Message "<< i << ": " << argv[i] << "\n";
                tempmsg += *argv[i] + ' '  ;
            }
        }
        msg.content = tempmsg;
    }
    std::cout << user.name << ": " << msg.content << "\n";
    
    return 0;
}