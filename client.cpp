#include "libtslog.h"
#include "client.h"
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <thread>

void client_thread_job(int id, const std::string& host, int port, const std::string& message) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        LOG_ERROR("Cliente " + std::to_string(id) + ": erro ao criar socket.");
        return;
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host.c_str(), &serv_addr.sin_addr) <= 0) {
        LOG_ERROR("Cliente " + std::to_string(id) + ": endereço inválido.");
        close(sock);
        return;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        LOG_ERROR("Cliente " + std::to_string(id) + ": falha na conexão.");
        close(sock);
        return;
    }

    LOG_INFO("Cliente " + std::to_string(id) + " conectado ao servidor.");

    std::string full_msg = "cliente#" + std::to_string(id) + ": " + message;
    send(sock, full_msg.c_str(), full_msg.size(), 0);

    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t n = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) {
            LOG_WARN("Cliente " + std::to_string(id) + " desconectado ou erro de rede.");
            break;
        }
        buffer[n] = '\0';
        LOG_INFO("Cliente " + std::to_string(id) + " recebeu: " + std::string(buffer));
    }

    close(sock);
    LOG_INFO("Cliente " + std::to_string(id) + " finalizado.");
}