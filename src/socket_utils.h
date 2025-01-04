#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include <iostream>
#include <string>
#include <vector>
#include <netinet/in.h>
#include <openssl/evp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


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

  int connection =
      connect(clientSocket, reinterpret_cast<sockaddr *>(&serverAddress),
              sizeof(serverAddress));
  if (connection == -1) {
    std::cerr << "Connection failed to server at " << server_ip << " on port "
              << port_no << ".\n";
    close(clientSocket);
    return -1;
  }
  std::cout << "Connected to server\n";
  return clientSocket;
}

#endif // SOCKET_UTILS_H