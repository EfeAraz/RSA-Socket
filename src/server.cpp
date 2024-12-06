#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h> //close()




int main(int argc, char **argv){
	int port_no = 8079;
    // AF_INET: Address Format_Internet = IP Addresses ()
    // PF_INET: Packet Format_Internet = IP, TCP/IP or UDP/IP 
	// fd: file descriptor
	int server_sockfd;	
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; // listen on all available interfaces
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_port = htons(port_no); 		// server port : 

    //create socketfd  with IPv4 and TCP 
    if((server_sockfd = socket(PF_INET,SOCK_STREAM,0)) < 0) {  			
		std::cerr << "socket creation failed\n"; 
					return 1;
	}
    /* bind socket with server addr */
	if(bind(server_sockfd,(struct sockaddr *)&server_addr,sizeof(struct sockaddr)) < 0) {
					std::cerr << "socket binding failed\n";
					return 1;
	}
    // listen connection request with queue lenght of 20
    if(listen(server_sockfd,20) < 0) {
					std::cerr << "listen error\n";
					return 1;
	}

	std::cerr << "listen success.\n" << "listening on port: " << port_no;

    char recv_buf[65536];
	memset(recv_buf, '\0', sizeof(recv_buf));
    
	while (1) {
		struct sockaddr_in client_addr;
		socklen_t length = sizeof(client_addr);
		// block on accept until positive fd or error
		int conn = accept(server_sockfd, (struct sockaddr*)&client_addr,&length);
		if(conn<0) {
			std::cerr << "couldn't connect";
			return -1;
		}

		std::cerr << "new client accepted.\n";

		char client_ip[INET_ADDRSTRLEN] = "";
		inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

		while(recv(conn, recv_buf, sizeof(recv_buf), 0) > 0 ){
			std::cerr << "recv "<< recv_buf << "from client:" << client_ip << ntohs(client_addr.sin_port) << "\n";
			memset(recv_buf, '\0', sizeof(recv_buf));
			break;
		}
	}



    std::cerr << "closing server. \n";
	close(server_sockfd);
    return 0;
}