#ifndef CLIENT_H
#define CLIENT_H

#include <string>

void client_thread_job(int id, const std::string& host, int port);

#endif