## TODO 
- [ ] Connect client to server and send messages  
- [ ] send encrypted messages to server  
- [ ] send public key to server  
- [ ] send public key from server to other client   
- [ ] implement sendmsg / sendmmsg (man)  
- [ ] Protection against DoS and other types of attacks   
---
# Description
C++ Project that uses OpenSSL's high level cryptographic functions (evp) to encyrpt/decrypt messages and send/recieve messages using sockets    

## Dependencies 

- OPENSSL

## Setting up for the first time

```bash 
sudo pacman -S openssl; # if you don't already have it installed  
git clone git@github.com:EfeAraz/RSA-Socket;  
cd RSA-Socket/;  
chmod +x keygen.sh;  
./keygen.sh;  
```


## Client :
```bash
g++ ./src/client.cpp -o ./bin/client -lssl -lcrypto;
./bin/client;  
```  

## Server :
```bash
g++ ./src/server.cpp -o ./bin/server;  
./bin/server  
```
### References:
- man command for manuals  
- [stackoverflow rsa encryption using keys](https://stackoverflow.com/questions/73631293/how-to-encrypt-a-string-using-openssl-c-library-and-a-public-key-file)    
- [Openssl docs](https://docs.openssl.org/master/man3/)  
- [communication protocols cpp implementations](https://commschamp.github.io/comms_protocols_cpp/)  

##
**This project is licensed under the terms of the MIT license.**  