## TODO
- [ ] Connect client to server and send messages  
- [ ] Encrypt messages and send to server  
- [ ] Generate encryption keys and save them    
- [ ] implement sendmsg / sendmmsg (man)  
- [ ] Protection against DoS and other types of attacks   

## Description
Sending Encrypted Messages using private and public keys with RSA encryption algorithm and network ports 

## Setting up for the first time
```bash
chmod +x compile.sh; ./compile.sh;  
chmod +x keygen.sh;  ./keygen.sh;  
```
## Client :
```bash
./bin/client
```  
## Server :
```bash
./bin/server  
```
### References:
man command for manuals  
[stackoverflow rsa encryption using keys](https://stackoverflow.com/questions/73631293/how-to-encrypt-a-string-using-openssl-c-library-and-a-public-key-file)  
[Openssl rsa_encrypt](https://github.com/openssl/openssl/blob/master/demos/encrypt/rsa_encrypt.c)  
[communication protocols cpp implementations](https://commschamp.github.io/comms_protocols_cpp/)  
 
##
**This project is licensed under the terms of the MIT license.**  