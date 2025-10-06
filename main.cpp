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
        std::string host = (argc >= 4) ? argv[3] : "127.0.0.1";
        int port = (argc >= 5) ? std::stoi(argv[4]) : 8080;

        client_thread_job(id, host, port);
    }
    else {
        std::cout << "Modo inválido. Use 'server' ou 'client'.\n";
    }

    return 0;
}