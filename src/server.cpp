#include "socket_utils.h"
#include "message_utils.h"
#include "cryption_utils.h"

int main(int argc, char **argv){
	int port_no = 8080;
	if (argc>1){
		// this is probably not safe practice but whatever
		port_no = atoi(argv[1]); // ./bin/server <port>
	}

	int server_sockfd = createServerSocket(port_no);

    char recv_buf[65536]; // buffer  2^16 because why not  
	memset(recv_buf, '\0', sizeof(recv_buf));
	sockaddr_in client_addr;
	socklen_t length = sizeof(client_addr);    
	while(true) {
		try{
			int conn = accept(server_sockfd, (sockaddr*)&client_addr,&length);
			if(conn<0) {
				std::cerr << "couldn't connect";
				close(server_sockfd);
				return -1;
			}

			std::cout << "\nnew client accepted." << std::endl;

			char client_ip[INET_ADDRSTRLEN] = "";
			inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

			while(recv(conn, recv_buf, sizeof(recv_buf), 0) > 0 ){
				std::cout << "recieved message:\n"<< recv_buf << "\n\nfrom client " << client_ip << ":" << ntohs(client_addr.sin_port) << std::endl;
				memset(recv_buf, '\0', sizeof(recv_buf));
			}
		}
		catch(const std::exception& e){
			std::cerr << "An error occured " << e.what() << "\n";
		}
	}

    std::cerr << "closing server. \n";
	close(server_sockfd);
    return 0;
}



