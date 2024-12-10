## TODO
-[] Connect client to server and send messages  
-[] Encrypt messages and send to server  
-[] Generate encryption keys and save them  

OSI model by layers                     | The TCP/IP Model  
7.  Application layer   probably not    }  
6.  Presentation layer  maybe           } Application Layer  
5.  Session layer       idk             }  
4.  Transport layer     probably    }Transport Layer  
3.  Network layer       probably    }Internet Layer  
2.  Data link layer     no                  }Network access layer  
1.  Physical layer      no                  }  

Socket:{  
    protocol,  
    local address,  
    local port,  
    remote address,  
    remote port  
    }  


  

## Setting up for the first time  
chmod +x compile.sh; ./compile.sh;  
chmod +x keygen.sh;  ./keygen.sh;  
# Client :
./bin/client  
# Server :
./bin/server  

### Description
Sending Encrypted Messages using RSA encryption algorithm and network ports 


# References:
https://stackoverflow.com/questions/73631293/how-to-encrypt-a-string-using-openssl-c-library-and-a-public-key-file  
https://en.wikipedia.org/wiki/Comparison_of_instant_messaging_protocols  
https://en.wikipedia.org/wiki/Communication_protocol  
https://commschamp.github.io/comms_protocols_cpp/  
https://github.com/openssl/openssl/blob/master/demos/encrypt/rsa_encrypt.c  
  
This project is licensed under the terms of the MIT license.  
  
##
