#ifndef CHATROOM_H
#define CHATROOM_H

#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>

class ChatRoom {
private:
    std::vector<std::string> messages;
    std::mutex mutex;
    std::condition_variable cv;
    bool new_message;

public:
    ChatRoom();

    // Adiciona mensagem e notifica consumidores
    void add_message(const std::string& message);

    // Espera até ter nova mensagem e retorna a última
    std::string wait_for_message();

    // Notifica entrada de cliente
    void client_joined(int client_id);

    // Notifica saída de cliente
    void client_left(int client_id);
};

#endif