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
	
	sockaddr_in client_addr;
	socklen_t length = sizeof(client_addr);
	
	bool keyACK = false;
	std::vector<std::string> keys(2);

	char recv_buf[65536] = {0 }; // buffer  2^16 because why not  
	while(true) {
		try{
			// connect to another socket 
			int conn = accept(server_sockfd, (sockaddr*)&client_addr,&length);
			if(conn<0) {
				std::cerr << "couldn't connect";
				close(server_sockfd);
				return -1;
			}

			std::cout << "new client accepted.\n" << std::endl;
			// https://beej.us/guide/bgnet/html/split/ip-addresses-structs-and-data-munging.html#:~:text=network%20to%20printable
			char client_ip[INET_ADDRSTRLEN] = "";
			inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

			while(recv(conn, recv_buf, sizeof(recv_buf), 0) > 0 ){
				if(!keyACK){
					keys[0] = recv_buf;
					std::cout << "Key recieved\n";
					keyACK = true;
					if(send(conn,recv_buf,sizeof(recv_buf),0) < 0){
						std::cout << "failed to send key to client!\n";
						keyACK = false;
					}
					else{
						std::cout << "Key sent to client\n";
					}
					memset(recv_buf, '\0', sizeof(recv_buf));
				}
				else{
					std::cout << "recieved message:\n"<< recv_buf << "\n\nfrom client " << client_ip << ":" << ntohs(client_addr.sin_port) << std::endl;
					memset(recv_buf, '\0', sizeof(recv_buf));

				}

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



