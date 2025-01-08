#include "socket_utils.h"
#include "message_utils.h"
#include <thread>
#include <unistd.h>


int port_no = 8080; // base port

// Vector to store connected clients
std::vector<client> clients;

// mutex for clients vector thread-safety
std::mutex clients_mutex;
int client_count = 0;
bool peersExists = false;
void handleClient(client& client_data);

int main(int argc, char **argv){
	if (argc>1){
		// this is probably not safe practice but whatever
		port_no = atoi(argv[1]); // ./bin/server <port>
	}
    int server_sockfd = createServerSocket(port_no);
	if (server_sockfd < 0) {
        std::cerr << "Failed to create server socket\n";
        return -1;
    }

	sockaddr_in client_addr;
	socklen_t length = sizeof(client_addr);

	while(true) {
		try{
			// https://beej.us/guide/bgnet/html/split/ip-addresses-structs-and-data-munging.html#:~:text=network%20to%20printable
			int conn = accept(server_sockfd, (sockaddr*)&client_addr,&length);
			if(conn<0) {
				std::cerr << "couldn't connect\n";
				close(server_sockfd);
				return -1;	
			}
            
			std::cout << "\nnew client accepted.\n";
			
			char client_ip[INET_ADDRSTRLEN] = "";
			inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
			client_count += 1;
			
			client new_client(conn, client_addr, client_ip);
			{
				std::lock_guard<std::mutex> lock(clients_mutex);
				clients.push_back(new_client);
			}
            std::thread(handleClient, std::ref(clients.back())).detach();
			std::cout << "Client IP: " << new_client.client_ip << ":" <<  ntohs(new_client.client_addr.sin_port) << "\n";
		}
		catch(const std::exception& e){
			std::cerr << "An error occured " << e.what() << "\n";
		}
	}
    std::cerr << "closing server. \n";
	close(server_sockfd);
    return 0;
}


void handleClient(client& client_data){
    char recv_buf[65536] = {0};
    // recieve data from client
    
    while(true){
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        if (!peersExists && clients.size() == 2 ) {
            std::lock_guard<std::mutex> lock(clients_mutex);

            client& client1 = clients[0];
            client& client2 = clients[1];
            client1.keyACK = false;
            client2.keyACK = false;
            // send client2's key to client1 
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            if (send(client1.conn, client2.key.c_str(), client2.key.size(), 0) > 0) {
                std::cout << "Sent public key of client " << client2.conn << " to client " << client1.conn << "\n";
            } else {
                std::cerr << "Failed to send key of client " << client2.conn << " to client " << client1.conn << "\n";
            }
        
            // send client1's key to client2
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            if (send(client2.conn, client1.key.c_str(), client1.key.size(), 0) > 0) {
                std::cout << "Sent public key of client " << client1.conn << " to client " << client2.conn << "\n";
            } else {
                std::cerr << "Failed to send key of client " << client1.conn << " to client " << client2.conn << "\n";
            }
            setPeer(&client1,&client2);
            peersExists = true;
        }

        while (recv(client_data.conn, recv_buf, sizeof(recv_buf), 0) > 0) {
            if(!client_data.keyACK){
                // replace client_data.conns with client_data.name later
                std::cout << "Recieving key from client: " << client_data.conn << "\n";
                client_data.key = recv_buf;
                std::cout << "Key recieved from client " << client_data.conn << "\n";
                client_data.keyACK = true;
            }
            else{
                std::cout << "\nRecieved message from client: " << client_data.conn << " ip: " << inet_ntoa(client_data.client_addr.sin_addr) << ":" << ntohs(client_data.client_addr.sin_port) << "\nMessage content:\n"<< recv_buf << std::endl;
                std::string recv_data = recv_buf;
                if(client_data.peer != nullptr){
                    if(send(client_data.peer->conn,recv_buf,sizeof(recv_buf),0) > 0){
                        std::cout << "Message sent to client(peer): " << client_data.peer->conn << std::endl;
                    }
                    else {
                        std::cerr << "Failed to send message to client(peer): " << client_data.peer << std::endl;
                    }
                }
                else if(send(client_data.conn,recv_buf,sizeof(recv_buf),0) > 0){
                    std::cout << "Peer doesn't exist for this client! Message sent back to client: " << client_data.conn << std::endl;; 
                }
                else{
                    std::cerr << "Message couldn't be sent to peer or the client: "<< client_data.conn << std::endl;
                }
            }
            memset(recv_buf, '\0', sizeof(recv_buf));
        }
        break;
    }
        std::cout << "\nClient disconnected: " << inet_ntoa(client_data.client_addr.sin_addr) << ":" << ntohs(client_data.client_addr.sin_port) << "\n";
        if(peersExists && client_data.peer!=nullptr)
            peersExists = false;
        close(client_data.conn);
        std::lock_guard<std::mutex> lock(clients_mutex);
        // remove the client from clients list

  
        client_count--;
        clients.erase(std::remove_if(clients.begin(), clients.end(),[&client_data](const client &c) { return c.conn == client_data.conn; }),clients.end());
        if(client_count == 0){
            std::cout << "No clients remaining\n"; 
        }
}
