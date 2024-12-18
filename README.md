## TODO
- [ ] CHANGE EVERY RSA KEYWORD TO PGP    
- [ ] Connect client to server and send messages  
- [ ] Encrypt messages and send to server  
- [ ] Generate encryption keys and save them    
- [ ] implement sendmsg / sendmmsg (man)  
- [ ] Protection against DoS and other types of attacks   
---
# Description
Sending Encrypted Messages using private and public keys with PGP or GPG encryption algorithm (haven't decided yet) and network ports  


## Dependencies 

- OPENSSL

## Setting up for the first time

```bash
git clone https://github.com/EfeAraz/RSA-Socket;  
sudo pacman -S openssl;  
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