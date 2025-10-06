#include "libtslog.h"
#include "client.h"
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

void client_thread_job(int id, const std::string& host, int port) {
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

    std::string input;
    while (true) {
        std::getline(std::cin, input);
        if (input.empty()) continue;
        if (input == "/sair") break;

        std::string msg = "cliente#" + std::to_string(id) + ": " + input;
        if (send(sock, msg.c_str(), msg.size(), 0) < 0) {
            LOG_WARN("Falha ao enviar mensagem.");
        }
    }

    close(sock);
    LOG_INFO("Cliente " + std::to_string(id) + " finalizado.");
}