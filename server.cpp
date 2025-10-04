#include "libtslog.h"
#include "server.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>

SimpleTCPServer::SimpleTCPServer(int port_) : server_fd(-1), port(port_) {}
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

    while (true) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket < 0) {
            LOG_ERROR(std::string("Erro no accept: ") + strerror(errno));
            continue;
        }

        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.push_back(client_socket);
        }

        std::thread(&SimpleTCPServer::handle_client, this, client_socket).detach();
    }

    return true;
}

void SimpleTCPServer::handle_client(int client_socket) {
    char buffer[1024];

    {
        std::string join_msg = ">>> Um novo cliente entrou na sala.";
        LOG_INFO(join_msg);

        std::lock_guard<std::mutex> lock(clients_mutex);
        for (int sock : clients) {
            if (sock != client_socket) {
                send(sock, join_msg.c_str(), join_msg.size(), 0);
            }
        }
    }

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t n = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) break;
        buffer[n] = '\0';

        LOG_INFO(std::string("Mensagem recebida: ") + buffer);

        std::lock_guard<std::mutex> lock(clients_mutex);
        for (int sock : clients) {
            if (sock != client_socket) {
                send(sock, buffer, strlen(buffer), 0);
            }
        }
    }

    // Aviso de saída do cliente
    {
        std::string leave_msg = "<<< Um cliente saiu da sala.";
        LOG_INFO(leave_msg);

        std::lock_guard<std::mutex> lock(clients_mutex);
        // remover cliente da lista primeiro
        clients.erase(std::remove(clients.begin(), clients.end(), client_socket), clients.end());

        // enviar aviso para os demais clientes
        for (int sock : clients) {
            send(sock, leave_msg.c_str(), leave_msg.size(), 0);
        }
    }

    close(client_socket);
    LOG_INFO("Conexão com cliente encerrada.");
}