## TODO 
- [ ] send public key from server to other client  
- [ ] get public keys, save it and send it to other client
- [ ] get public key from server
- [ ] protection against DoS and other security related attacks  
- [ ] add salt to messages/keys  
---
# Description
C++ Project that uses OpenSSL's high level cryptographic functions (evp) to encyrpt/decrypt messages and send/recieve messages using sockets    
## Dependencies 
- Unix shell  
- OPENSSL  
# Setting up for the first time

```bash 
git clone git@github.com:EfeAraz/RSA-Socket;  
chmod +x setup.sh keygen.sh;  
./setup.sh;  
```


## Client :
```bash
g++ ./src/client.cpp -o ./bin/client -lssl -lcrypto;
./bin/client 8080;  
```  

## Server :
```bash
g++ ./src/server.cpp -o ./bin/server;  
./bin/server 8080;
```
##
### References:
- man command for manuals  
- [stackoverflow rsa encryption using keys](https://stackoverflow.com/questions/73631293/how-to-encrypt-a-string-using-openssl-c-library-and-a-public-key-file)    
- [Openssl docs](https://docs.openssl.org/master/man3/)  
- [communication protocols cpp implementations](https://commschamp.github.io/comms_protocols_cpp/)  


---
### **This project is licensed under the terms of the MIT license.**  