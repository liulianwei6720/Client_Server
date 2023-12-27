#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <unordered_map>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <unordered_map>

class Server
{
public:
    Server(void);
    ~Server(void);
    void Work(void);
private:
    char *GetComputerName(void);
    char *GetTime(void);
    void GetClients(void);
    void SendTo(void);
    void Accept(void);
    void ListenToClient(void);
    int sockfd_;
    sockaddr_in address_;
    std::unordered_map<int, sockaddr_in> *clients_;
    
};

#endif