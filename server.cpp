#include "server.h"
#include "libtslog.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <algorithm>
#include <queue>

SimpleTCPServer::SimpleTCPServer(int port_)
    : server_fd(-1), port(port_), connected_clients(0), next_client_id(1) {}

SimpleTCPServer::~SimpleTCPServer() {
    if (server_fd != -1) close(server_fd);
}

bool SimpleTCPServer::start() {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        LOG_ERROR(std::string("Erro ao criar socket: ") + strerror(errno));
        return false;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        LOG_ERROR(std::string("Erro no bind: ") + strerror(errno));
        return false;
    }

    if (listen(server_fd, 10) < 0) {
        LOG_ERROR(std::string("Erro no listen: ") + strerror(errno));
        return false;
    }

    LOG_INFO("Servidor iniciado na porta " + std::to_string(port));

    std::thread([this] {
        while (true) {
            std::string msg = chatroom.wait_for_message();
            LOG_INFO(msg);
            broadcast(msg);
        }
    }).detach();

    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket < 0) {
            LOG_ERROR(std::string("Erro no accept: ") + strerror(errno));
            continue;
        }

        int client_id;
        {
            std::unique_lock<std::mutex> lock(clients_mutex);
            cv_slots.wait(lock, [this]{ return connected_clients < MAX_CLIENTS; });
            connected_clients++;

            if (!free_ids.empty()) {
                client_id = free_ids.front();
                free_ids.pop();
            } else {
                client_id = next_client_id++;
            }

            LOG_INFO("Cliente conectado! Slots restantes: " + std::to_string(MAX_CLIENTS - connected_clients));
        }

        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.push_back(client_socket);
        }

        chatroom.client_joined(client_id);

        std::thread([this, client_socket, client_id]() {
            handle_client(client_socket, client_id);

            {
                std::lock_guard<std::mutex> lock(clients_mutex);
                connected_clients--;
                free_ids.push(client_id);
                LOG_INFO("Cliente #" + std::to_string(client_id) + " saiu! Slots restantes: " +
                         std::to_string(MAX_CLIENTS - connected_clients));
            }

            cv_slots.notify_one();
        }).detach();
    }

    return true;
}

void SimpleTCPServer::handle_client(int client_socket, int client_id) {
    char buffer[1024];

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t n = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) break;
        buffer[n] = '\0';

        std::string msg(buffer);
        chatroom.add_message(msg);
    }

    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.erase(std::remove(clients.begin(), clients.end(), client_socket), clients.end());
    }

    chatroom.client_left(client_id);
    close(client_socket);
}

void SimpleTCPServer::broadcast(const std::string& msg) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (int client : clients) {
        send(client, msg.c_str(), msg.size(), 0);
    }
}