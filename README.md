## TODO 
- [ ] logs  
- [ ] improve functions  
- [ ] maybe add salt to messages/keys  
---
# Description
C++ Project that uses OpenSSL's cryptographic functions to encyrpt/decrypt messages and send/recieve messages using [sockets](https://www.geeksforgeeks.org/socket-in-computer-network/).  

You can use your existing [OpenSSL compatible](https://wiki.openssl.org/index.php/EVP#:~:text=The%20following%20EVP_PKEY%20types%20are%20supported%3A) public/private key pair or generate a new one with keygen.sh  

## Dependencies  
- Unix shell  
- OPENSSL

# Setting up for the first time  
#### install openssl from your package manager  
```bash 
git clone https://github.com/EfeAraz/RSA-Socket;  
chmod +x setup.sh keygen.sh;  
./setup.sh;   
```

## Server :
```bash
g++ ./src/server.cpp -o ./bin/server;./bin/server 8080;
```

## Client :
example for localhost:
```bash
g++ ./src/client.cpp -o ./bin/client -lssl -lcrypto;./bin/client 127.0.0.1 8080 username; 
```  


---
### References:
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)  
- man/tldr pages  
- [geeksforgeeks](https://www.geeksforgeeks.org/socket-programming-cc/)  
- [stackoverflow rsa encryption using keys](https://stackoverflow.com/questions/73631293/how-to-encrypt-a-string-using-openssl-c-library-and-a-public-key-file)    
- [Openssl docs](https://docs.openssl.org/master/man3/)  
- [communication protocols cpp implementations](https://commschamp.github.io/comms_protocols_cpp/)  
---
### **This project is licensed under the terms of the MIT license.**  
