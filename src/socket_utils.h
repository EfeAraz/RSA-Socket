#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include <iostream>
#include <cstring>
#include <netinet/in.h>
#include <openssl/evp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
constexpr int max_client_count = 3;

//returns server socket file descriptor
extern inline int createServerSocket(int port_no){
  int server_sockfd = -1;			// socket file descriptor 
    sockaddr_in server_addr; 	// internet socket address descriptor 
    memset(&server_addr, 0, sizeof(server_addr)); // clear garbage data   
    
    server_addr.sin_family = AF_INET; 	// IPv4
    server_addr.sin_port = htons(port_no); // set port for server   
      server_addr.sin_addr.s_addr = INADDR_ANY; // listen on all available interfaces

    
    // if --- is not done close socket exit
      //create socket file descriptor, 
      if((server_sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {  			
      std::cerr << "socket creation failed\n"; 
      close(server_sockfd);
      return 1;
    }
      // bind socket 
    if(bind(server_sockfd,(sockaddr*)&server_addr,(socklen_t)sizeof(sockaddr)) < 0) {
      std::cerr << "socket binding failed\n";
      close(server_sockfd);
      return 1;
    }
      // listen on socket
      if(listen(server_sockfd,max_client_count) < 0) { 
      std::cerr << "listening failed\n";
      close(server_sockfd);
      return 1;
    }
    std::cout << "listening on port: " << port_no << std::endl;
    return server_sockfd;
}



// returns connected socket descriptor on successful completion, otherwise
// returns -1
extern inline int connectToServer(std::string &server_ip, int port_no) {
  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (clientSocket == -1) {
    std::cerr << "Socket creation failed\n";
    return -1;
  }

  sockaddr_in serverAddress;
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_port = htons(port_no);
  // Convert the ip address
  if (inet_pton(AF_INET, server_ip.c_str(), &serverAddress.sin_addr) <= 0) {
    std::cerr << "Invalid IP address: " << server_ip << "\n";
    close(clientSocket);
    return -1;
  }

  int connection = connect(clientSocket, reinterpret_cast<sockaddr *>(&serverAddress),sizeof(serverAddress));
  if (connection == -1) {
    std::cerr << "Connection failed to server at " << server_ip << " on port " << port_no << ".\n";
    close(clientSocket);
    return -1;
  }
  std::cout << "Connected to server\n";
  return clientSocket;
}

#endif // SOCKET_UTILS_H