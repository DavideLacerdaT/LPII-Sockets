#ifndef SERVER_H
#define SERVER_H

#include <vector>
#include <string>
#include <mutex>
#include <queue>
#include "chatroom.h"

class SimpleTCPServer {
private:
    int server_fd;
    int port;
    std::vector<int> clients;
    std::mutex clients_mutex;

    std::mutex slots_mutex;
    std::condition_variable cv_slots;
    int connected_clients;
    static const int MAX_CLIENTS = 5;

    ChatRoom chatroom;
    int next_client_id;

    std::queue<int> free_ids;

public:
    SimpleTCPServer(int port_);
    ~SimpleTCPServer();

    bool start();
    void handle_client(int client_socket, int client_id);
    void broadcast(const std::string& msg);
};

#endif