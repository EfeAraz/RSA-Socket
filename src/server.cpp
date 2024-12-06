#include <iostream>
#include <ostream>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> //close()

#define port_no 8079
#define max_client_count 3




int main(int argc, char **argv){
	// #define AF_INET PF_INET
    // AF_INET: Address Format_Internet = IP Addresses ()
    // PF_INET: Packet Format_Internet = IP, TCP/IP or UDP/IP 
	

	int server_sockfd;	// socket file descriptor 
    sockaddr_in server_addr; // internet socket address descriptor 
	memset(&server_addr, 0, sizeof(server_addr)); // server_addr : Null  
    server_addr.sin_family = AF_INET; 	// IPv4
	server_addr.sin_port = htons(port_no); // set port for server   
    server_addr.sin_addr.s_addr = INADDR_ANY; // listen on all available interfaces


	// do --- , check if --- is done
    //create socketfd with IPv4 and TCP, 
    if((server_sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0) {  			
		std::cerr << "socket creation failed\n"; 
		close(server_sockfd);
		return 1;
	}
    // bind socket with server addr 
	if(bind(server_sockfd,(sockaddr*)&server_addr,(socklen_t)sizeof(sockaddr)) < 0) {
		std::cerr << "socket binding failed\n";
		close(server_sockfd);
		return 1;
	}
    // 
    if(listen(server_sockfd,max_client_count) < 0) { 
		std::cerr << "listen error\n";
		close(server_sockfd);
		return 1;
	}

	std::cout << "listening on port: " << port_no << std::endl;

    char recv_buf[65536]; // 2^16 because why not  
	memset(recv_buf, '\0', sizeof(recv_buf));
    
	while (1) {
		sockaddr_in client_addr;
		socklen_t length = sizeof(client_addr);
		// block on accept until positive fd or error
		int conn = accept(server_sockfd, (sockaddr*)&client_addr,&length);
		if(conn<0) {
			std::cerr << "couldn't connect";
			return -1;
		}

		std::cout << "new client accepted." << std::endl;

		char client_ip[INET_ADDRSTRLEN] = "";
		inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

		while(recv(conn, recv_buf, sizeof(recv_buf), 0) > 0 ){
			std::cout << "recv "<< recv_buf << "from client:" << client_ip << ntohs(client_addr.sin_port) << std::endl;
			memset(recv_buf, '\0', sizeof(recv_buf));
			break;
		}
	}



    std::cerr << "closing server. \n";
	close(server_sockfd);
    return 0;
}