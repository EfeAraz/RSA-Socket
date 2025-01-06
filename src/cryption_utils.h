#ifndef CRYPTO_UTILS_H
#define CRYPTO_UTILS_H

#include <iostream>
#include <string>
#include <vector>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>

// Don't modify the functions under this line if you're not especially using with them 
extern inline void handleOpenSSLError(){
    ERR_print_errors_fp(stderr);
    abort();
}

extern inline EVP_PKEY* loadPublicKey(const std::string& publicKeyFile){
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

extern inline std::string publicKeyToPEMString(EVP_PKEY* publicKey) {
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

extern inline EVP_PKEY* loadPrivateKey(const std::string& privateKeyFile){
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


extern inline std::vector<unsigned char> encryptWithPublicKey(EVP_PKEY* publicKey,const std::string& text){
    // read key content  
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


extern inline std::string decryptWithPrivateKey(EVP_PKEY* privateKey, const std::vector<unsigned char>& encryptedText) {
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


extern inline void freeKey(EVP_PKEY *key){
    EVP_PKEY_free(key);
}

extern inline std::string base64Encode(const std::vector<unsigned char>& binary) {
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


extern inline std::vector<unsigned char> base64Decode(const std::string& base64Text) {
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
 

#endif // CRYPTO_UTILS_H