#include "chatroom.h"
#include "libtslog.h"
#include <mutex>
#include <condition_variable>
#include <string>
#include <vector>

ChatRoom::ChatRoom() : new_message(false) {}

void ChatRoom::add_message(const std::string& message) {
    std::unique_lock<std::mutex> lock(mutex);
    messages.push_back(message);
    new_message = true;

    cv.notify_all();
}

std::string ChatRoom::wait_for_message() {
    std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock, [this]{ return new_message; });
    new_message = false;
    return messages.back();
}

void ChatRoom::client_joined(int client_id) {
    std::unique_lock<std::mutex> lock(mutex);
    std::string msg = ">>> Cliente " + std::to_string(client_id) + " entrou no chat.";
    messages.push_back(msg);
    new_message = true;

    cv.notify_all();
}

void ChatRoom::client_left(int client_id) {
    std::unique_lock<std::mutex> lock(mutex);
    std::string msg = "<<< Cliente " + std::to_string(client_id) + " saiu do chat.";
    messages.push_back(msg);
    new_message = true;

    cv.notify_all();
}