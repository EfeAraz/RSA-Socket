#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <ctime>
#include <arpa/inet.h>


int port = 8080;
std::string serverIP = "192.168.1.1"; 
int clientSocket = -1;

const std::string privateKeyFile = "./keys/private.pem";
const std::string publicKeyFile = "./keys/public.pem"; 
const std::string otherPublicKeyFile = "./keys/public.pem"; // get this key from server

// FUNCTION DECLARATIONS 
int connectToServer(std::string& serverIP,int port_no); // returns socket/file descriptor upon successful completion, otherwise returns -1   
void logMessage(std::string logMessage);
int sendMessage(int ClientSocket, std::string message); // returns -1 upon fail
std::vector<unsigned char> encryptWithPublicKey(EVP_PKEY* publicKey,const std::string& text); // returns encrypted message in binary format  
std::string decryptWithPrivateKey(EVP_PKEY* privateKey, const std::vector<unsigned char>& encryptedText); // binary ->  
std::string base64Encode(const std::vector<unsigned char>& binary); // binary -> base64
std::vector<unsigned char> base64Decode(const std::string& base64Text);
std::string publicKeyToPEMString(EVP_PKEY* publicKey); // public key file contents -> public pem key

// Don't modify the functions under this line if you're not especially using with them 
void handleOpenSSLError(); // core dump
EVP_PKEY* loadPublicKey(const std::string& publicKeyFile);      // delete this function after implementing savePublicKey
EVP_PKEY* loadPrivateKey(const std::string& privateKeyFile);    // read private key into openssl's private key data structure
void sendPublicKeyToServer(int clientSocket,EVP_PKEY* publicKey,int port_no);

/*  NOT IMPLEMENTED YET
* std::vector<std::string> readFileIntoString(std::string &fileAddress); 
* void savePublicKey(std::vector<std::string> publicKey,std::string fileAddress); // get public key from server and save it into a variable
* EVP_PKEY* savePublicKey(std::vector<std::string> publicKey,std::string fileAddress);
*
*/
// FUNCTION DECLARATIONS END


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
    clientSocket = connectToServer(serverIP,port);
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
    
    //     EVP_PKEY_free(publicKey); // free the local public key
    //  recieve other public key from server 

    // sending messages
    try{
        while (true) {     
            std::string message = "";
            std::cout << "Enter your message (type 'exit' or press Ctrl+C to quit): ";
            std::getline(std::cin, message);

            if (message == "exit") {
                break;
            }

            std::vector<unsigned char> encrypted = encryptWithPublicKey(publicKey, message);  
            std::string base64Message = base64Encode(encrypted);

            std::cout << "\nEncrypted message in base64 format:\n" << base64Message << "\n";
            
            /*
            * std::vector<unsigned char> base64DecodededMessage = base64Decode(base64Message);
            * std::string decryptedMessage =  decryptWithPrivateKey(privateKey,base64DecodededMessage);
            * std::cout << "\nDecrpyted message: " << decryptedMessage << std::endl;
            */
            sendMessage(clientSocket,base64Message);
            sleep(1);
        }
    }
    catch(const std::exception& e){
        std::cerr << "An error occured " << e.what() << "\n";

    }
    close(clientSocket);
    EVP_PKEY_free(publicKey);
    EVP_PKEY_free(privateKey); 
    return 0;
}


// returns connected socket descriptor on successful completion, otherwise returns -1
int connectToServer(std::string& server_ip,int port_no){
    int clientSocket = socket(AF_INET ,SOCK_STREAM ,0);
    if(clientSocket == -1){
        std::cerr << "Socket creation failed\n";
        return -1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port_no);
    // Convert the ip address 
    if (inet_pton(AF_INET, server_ip.c_str(), &serverAddress.sin_addr) <= 0) {
        std::cerr << "Invalid IP address: " << server_ip << "\n";
        close(clientSocket);
        return -1;
    }

    int connection = connect(clientSocket ,reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress));
    if(connection == -1){
        std::cerr << "Connection failed to server at " << server_ip << " on port " << port_no << ".\n";
        close(clientSocket);
        return -1;
    }
    std::cout << "Connected to server\n";
    return clientSocket;
}

void logMessage(std::string logMessage){


}

int sendMessage(int clientSocket,std::string message){
    const char* msg = message.c_str();
    if(send(clientSocket,msg,strlen(msg),0) >= 0){
        std::cout <<  "Message Sent!\n";
        return 1;
    }
    std::cerr << "Couldn't send message\n";
    return -1;
}


std::vector<unsigned char> encryptWithPublicKey(EVP_PKEY* publicKey,const std::string& text){
    // key context 
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


std::string decryptWithPrivateKey(EVP_PKEY* privateKey, const std::vector<unsigned char>& encryptedText) {
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


std::string base64Encode(const std::vector<unsigned char>& binary) {
    BIO* bio = BIO_new(BIO_s_mem());
    BIO* b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    // Disable newline breaks in Base64 encoding
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

    BIO_write(bio, binary.data(), binary.size());
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


std::string publicKeyToPEMString(EVP_PKEY* publicKey) {
    BIO* bio = BIO_new(BIO_s_mem());
    if (!PEM_write_bio_PUBKEY(bio, publicKey)) {
        BIO_free(bio);
        std::cerr << "Failed to convert public key into string format\n";
    }
    char* pemData;
    long pemLength = BIO_get_mem_data(bio, &pemData);
    std::string pemString(pemData, pemLength);

    BIO_free(bio);
    return pemString;
}


// Don't modify the functions under this line if you're not especially using with them 
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


void sendPublicKeyToServer(int clientSocket,EVP_PKEY* publicKey,int port_no){
    std::string pemKey = publicKeyToPEMString(publicKey);
    if(sendMessage(clientSocket,pemKey) < 0){
        std::cerr << "Couldn't Send Key to Server\n";
        return;
    }
    std::cout << "Key Sent\n";
}

// THINGS THAT ARE NOT IMPLEMENTED YET

std::vector<std::string> readFileIntoString(std::string &fileAddress){
    std::vector<std::string> output;
    std::cout << "Reading: " << fileAddress << "\n" ;
    std::ifstream fileStream(fileAddress);
    std::string temp;
    while(std::getline(fileStream,temp)){
        output.push_back(temp);
    }
    std::cout << "File contents:\n";
    for (const auto& line : output){
        std::cout << line << std::endl;
    }
    return output;
}


void savePublicKey(std::vector<std::string> publicKey,std::string fileAddress){
    
    std::ofstream keyFile(fileAddress); // create file
    keyFile.clear();
     for (const auto& line :publicKey){
        std::string text;
        keyFile << line;
    }
}