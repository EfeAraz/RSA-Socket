#include <cstdio>
#include <iostream>
#include <string>
// #include <fstream> // for file transfers 
#include <vector>
#include <sys/socket.h>
#include <openssl/evp.h>

#include <openssl/pem.h>
#include <openssl/err.h>

// TO-DO
// -Fix decrypt-encrypt file location
// -Send public key to server
// -Send Encyrpted messages to server 
// -Change every rsa to pgp
void handleOpenSSLError(){
    ERR_print_errors_fp(stderr);
    abort();
}

/*
 * DONT USE THIS FUNCTION YET
 * delete this comment section if you change the function in the bottom
 */  
EVP_PKEY* loadPublicKey(const std::string& publicKeyFile){
    FILE* fp = fopen(publicKeyFile.c_str(), "r");
    if(!fp){
        std::cerr << "Error: Couldn't open public key file\n";
        fclose(fp); //just in case if exiting doesnt close it
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
        fclose(fp); //just in case if exiting doesnt close it
        exit(1);
    }
    
    EVP_PKEY* privateKey = PEM_read_PrivateKey(fp,nullptr,nullptr,nullptr ); 
    fclose(fp);

    if(!privateKey){
        std::cerr << "Error: Couldn't read public key\n";
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

    // Determine buffer size for the decrypted output
    if (EVP_PKEY_decrypt(ctx, nullptr, &outlen, encryptedText.data(), encryptedText.size()) <= 0)
        handleOpenSSLError();

    std::vector<unsigned char> decrypted(outlen);

    // decryption
    if (EVP_PKEY_decrypt(ctx, decrypted.data(), &outlen, encryptedText.data(), encryptedText.size()) <= 0)
        handleOpenSSLError();

    EVP_PKEY_CTX_free(ctx); // Clean up

    return std::string(decrypted.begin(), decrypted.end());
}

int main(int argc,char** argv){
    
    const std::string publicKeyFile = "./keys/public.pem";
    const std::string privateKeyFile = "./keys/private.pem";

    const std::string message = "hi i'm arz";

    EVP_PKEY* publicKey = loadPublicKey(publicKeyFile);
    EVP_PKEY* privateKey = loadPrivateKey(privateKeyFile);
    if (!publicKey || !privateKey) {
        std::cerr << "Error: Couldn't read key\n";
        return 1;
    }

    std::vector<unsigned char> encrypted = encryptWithPublicKey(publicKey, message); 
    
    std::cout << "Encrypted message in hex format: ";
    for(unsigned char c : encrypted){
        printf("%02x", c);
        //  std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c);
    }
    std::cout << std::endl;


    std::string decryptedMessage =  decryptWithPrivateKey(privateKey,encrypted);
    std::cout << "Decrpyted message: " << decryptedMessage << std::endl;

    std::cout << std::endl;
    EVP_PKEY_free(publicKey);
    EVP_PKEY_free(privateKey); 
    return 0;
}


