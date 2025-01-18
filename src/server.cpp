#include "socket_utils.h"
#include "message_utils.h"
#include <thread>
#include <unistd.h>


int port_no = 8080; // base port

// Vector to store connected clients
std::vector<client> clients;

// mutex for clients vector thread-safety
std::mutex clients_mutex;
std::vector<std::thread> thread_clients;
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
            // Create new client on connection
			std::cout << "\nnew client accepted.\n";
			
            char client_ip[INET_ADDRSTRLEN] = "";
			inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

			client new_client(conn, client_addr, client_ip);
			{
				std::lock_guard<std::mutex> lock(clients_mutex);
				clients.push_back(new_client);
                std::cout << "Client " << new_client.conn << " IP: " << new_client.client_ip << ":" <<  ntohs(new_client.client_addr.sin_port) << "\n";
			}
            std::thread newthread(handleClient, std::ref(clients.back()));
            newthread.detach();
            thread_clients.push_back(std::move(newthread));
            /*
            std::cout << "clients.size() = " << clients.size() << std::endl;
            if(clients.size() == 2){
                std::lock_guard<std::mutex> lock(clients_mutex);
                client* pair = new_client.peer;
                std::this_thread::sleep_for(std::chrono::seconds(15));
                auto pair_loc = std::find_if(clients.begin(), clients.end(),[pair](const client& c) { return &c == pair; });
                if(pair_loc != clients.end()){
                    std::lock_guard<std::mutex> lock(clients_mutex);
                    int pair_distance = std::distance(clients.begin(),pair_loc);
                    client old_client = clients[pair_distance];
                    thread_clients[pair_distance].~thread();
                    std::thread new_thread_for_old_client(handleClient, std::ref(old_client));
                    new_thread_for_old_client.detach();
                    thread_clients.push_back(std::move(new_thread_for_old_client));
                    std::cout << "Peer Relationship established, old thread has been recreated\n";
            
                }
                else{
                    std::cerr << "Client peer not found for client: " << new_client.conn << std::endl; 
                }
            }
            */
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
        while (recv(client_data.conn, recv_buf, sizeof(recv_buf), 0) > 0) {
            if(!client_data.keyACK){
                std::cout << "Recieving key from client: " << client_data.conn << "\n";
                client_data.key = recv_buf;
                std::cout << "Key recieved from client " << client_data.conn << "\n";
                client_data.keyACK = true;
                if (clients.size() == 2 && !client_data.peerKeyACK) {
                    std::lock_guard<std::mutex> lock(clients_mutex);
                    client& client1 = clients[0];
                    client& client2 = clients[1];
                    if(client1.keyACK && client2.keyACK){
                        client1.keyACK = false;
                        client2.keyACK = false;
                        if (send(client1.conn, client2.key.c_str(), client2.key.size(), 0) > 0) {
                            std::cout << "\nSent public key of client " << client2.conn << " to client " << client1.conn << "\n";
                            client1.keyACK = true;
                        } 
                        else {
                            std::cerr << "Failed to send key of client " << client2.conn << " to client " << client1.conn << "\n";
                            client1.keyACK = false;
                            client2.keyACK = false;
                            break;
                        }
                    
                        // send client1's key to client2
                        if (send(client2.conn, client1.key.c_str(), client1.key.size(), 0) > 0) {
                            std::cout << "Sent public key of client " << client1.conn << " to client " << client2.conn << "\n";
                            client2.keyACK = true;
                        } 
                        else {
                            std::cerr << "Failed to send key of client " << client1.conn << " to client " << client2.conn << "\n";
                            client1.keyACK = false;
                            client2.keyACK = false;
                            break;
                        }
                        setPeer(&client1,&client2);
                        peersExists = true;
                        client_data.keyACK = true;
                        client_data.peerKeyACK = true;


                        client* pair = client_data.peer;
                        auto pair_loc = std::find_if(clients.begin(), clients.end(),[pair](const client& c) { return &c == pair; });
                        if(pair_loc != clients.end()){
                            std::cout << "Pair found!\n";
                            int pair_distance = std::distance(clients.begin(),pair_loc);
                            client old_client = clients[pair_distance];
                            std::cout << "Deleting pair's thread\n";
                            thread_clients[pair_distance].~thread();
                            std::cout << "Creating new thread for the pair\n";
                            std::thread new_thread_for_old_client(handleClient, std::ref(old_client));
                            new_thread_for_old_client.detach();
                            thread_clients.push_back(std::move(new_thread_for_old_client));
                            std::cout << "Pair Relationship established for sure!, old thread has been recreated\n";
                    
                        }
                        else{
                            std::cerr << "Client pair not found for client: " << client_data.conn << std::endl; 
                        }
                    }
                }
            }
            else{
                std::cout << "\nRecieved message from client: " << client_data.conn << " ip: " << inet_ntoa(client_data.client_addr.sin_addr) << ":" << ntohs(client_data.client_addr.sin_port) << "\nMessage content:\n"<< recv_buf << std::endl;
                std::string recv_data = recv_buf;
                std::string peer_fail_message = "Failed to send message to peer";
                if(client_data.peer != nullptr){
                    if(send(client_data.peer->conn,recv_buf,sizeof(recv_buf),0) > 0){
                        std::cout << "Message sent to client(peer): " << client_data.peer->conn << std::endl;
                    }
                    else {
                        std::cerr << "Failed to send message to client(peer): " << client_data.peer << std::endl;
                    }
                }
                else if(send(client_data.conn,peer_fail_message.c_str(),sizeof(recv_buf),0) > 0){
                    std::cout << "Peer doesn't exist for this client! Error Message sent to client: " << client_data.conn << std::endl;; 
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
        if(client_data.peerKeyACK && peersExists){
            peersExists = false;
            client_data.keyACK = false;
            client_data.peerKeyACK = false;
        }
        close(client_data.conn);
        std::lock_guard<std::mutex> lock(clients_mutex);
        // remove the client from clients list        
        clients.erase(std::remove_if(clients.begin(), clients.end(),[&client_data](const client &c) { return c.conn == client_data.conn; }),clients.end());
        if(clients.size() == 0){
            std::cout << "No clients remaining\n"; 
        }
}


