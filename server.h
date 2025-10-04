#pragma once
#include <string>
#include <vector>
#include <mutex>

class SimpleTCPServer {
    int server_fd;
    int port;

    std::vector<int> clients;
    std::mutex clients_mutex;

public:
    SimpleTCPServer(int port_);
    ~SimpleTCPServer();
    
    bool start();

private:
    void handle_client(int client_socket);
    void broadcast_message(const std::string& message, int sender_socket);
};