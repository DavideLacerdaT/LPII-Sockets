#include "libtslog.h"
#include "server.h"
#include "client.h"
#include <string>
#include <iostream>

int main(int argc, char** argv) {
    if (argc == 1) {
        system("./run_example.sh");
        return 0;
    }

    std::string mode = argv[1];

    if (mode == "server") {
        int port = (argc >= 3) ? std::stoi(argv[2]) : 8080;
        SimpleTCPServer server(port);
        server.start();
    } 
    else if (mode == "client") {
        int id = (argc >= 3) ? std::stoi(argv[2]) : 1;
        std::string message = (argc >= 4) ? argv[3] : "Olá!";
        std::string host = (argc >= 5) ? argv[4] : "127.0.0.1";
        int port = (argc >= 6) ? std::stoi(argv[5]) : 8080;

        client_thread_job(id, host, port, message);
    } 
    else {
        std::cout << "Modo inválido. Use 'server' ou 'client'.\n";
    }

    return 0;
}