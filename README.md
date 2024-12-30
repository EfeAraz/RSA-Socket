## TODO 
- [ ] connection.h  
- [ ] cryption?.h
- [ ] logs
- [ ] send public key from server to other client  
- [ ] get public keys, save it and send it to other client
- [ ] get public key from server
- [ ] implement other settings like file transfer  
- [ ] add salt to messages/keys  
- [ ] protection against DoS and other security related attacks  

---
# Description
C++ Project that uses OpenSSL's cryptographic functions to encyrpt/decrypt messages and send/recieve messages using [sockets](https://www.geeksforgeeks.org/socket-in-computer-network/).  

You can use your existing [OpenSSL compatible](https://wiki.openssl.org/index.php/EVP#:~:text=The%20following%20EVP_PKEY%20types%20are%20supported%3A) public/private key pair or generate a new one with keygen.sh  

## Dependencies 
- Unix shell  
- OPENSSL

# Setting up for the first time
```bash 
git clone https://github.com/EfeAraz/RSA-Socket;  
chmod +x setup.sh keygen.sh;  
./setup.sh;  
```

## Client :
```bash
 g++ ./src/client.cpp -o ./bin/client -lssl -lcrypto;./bin/client 8080; 
```  

## Server :
```bash
g++ ./src/server.cpp -o ./bin/server;./bin/server 8080;
```
---
### References:
- man command for manuals  
- [geeksforgeeks](https://www.geeksforgeeks.org/socket-programming-cc/)  
- [stackoverflow rsa encryption using keys](https://stackoverflow.com/questions/73631293/how-to-encrypt-a-string-using-openssl-c-library-and-a-public-key-file)    
- [Openssl docs](https://docs.openssl.org/master/man3/)  
- [communication protocols cpp implementations](https://commschamp.github.io/comms_protocols_cpp/)  


---
### **This project is licensed under the terms of the MIT license.**  