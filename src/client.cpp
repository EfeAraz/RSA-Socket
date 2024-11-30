#include <fstream>
#include <iostream>
#include <string>
// TO-DO
// -Fix decrypt-encrypt file location

class message{
    public:
        int sender_id;
        std::string content;

};

class client{
    private:
        std::string private_key;
        std::string public_key;

    public:
    client(){
        public_key = readFile("keys/public.key");
        private_key = readFile("keys/private.key");
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
    


    message encrypt(message msg){
        message encrypted_msg;
        return encrypted_msg;
    }

    message decrypt(message msg){
        message decrypted_msg;

        return decrypted_msg;
    }
};


int main(int argc,char** argv){
    message msg;
    client user;
    if(argc <= 1){ 
        std::cout << "Enter Your Message: " << "\n";
        std::getline(std::cin,msg.content);
    }
    else{
        std::string tempmsg;
        for(int i = 1; i < argc; i++){
            std::cout << "Message "<< i << ": " << argv[i] << "\n";
            tempmsg += argv[i];
        }
        msg.content = tempmsg;
    }
    

    return 0;
}