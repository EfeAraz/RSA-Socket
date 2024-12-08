/*
        THIS IS NOT A GOOD PRACTICE
        USE THE BUILT-IN RSA ALGORITHM FROM COMMAND LINE   
*/
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <iostream>
std::string Generate_Private_Key();
std::string Generate_Public_Key(std::string private_key);
int Generate_Matching_Keys();

int main(int argc,char** argv){

std::string public_key{};
std::cout << public_key;
return 0;
}


int Generate_Matching_Keys(){
// Generate private_key 
std::string private_key = Generate_Private_Key();
std::string public_key = Generate_Public_Key(private_key);
// Generate public_key using private_key
// Save Keys
return 0;
}

std::string Generate_Private_Key(){
        std::string private_key{};
        // Generate private_key
        return private_key;
}

std::string Generate_Public_Key(std::string private_key){
        std::string public_key{};
        // Generate public_key
        return public_key;
}