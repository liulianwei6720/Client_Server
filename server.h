#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <unordered_map>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <unordered_map>

struct ClientElement
{
    int socketfd_;
    sockaddr_in addr;
    ClientElement(int skt, sockaddr_in &ad)
    {
        socketfd_ = skt;
        addr = ad;
        return;
    }
};

class Server
{
public:
    Server(void);
    ~Server(void);
    void Work(void);
private:
    bool GetComputerName(char *buf, size_t buf_size);
    bool GetClients(char *buf, size_t buf_size);
    void SendTo(int skt);
    void ListenToClient(int skt);
    void DeleteClient(int skt);
    void Serialize(size_t ptr, char *dst, size_t dst_size, sockaddr_in &skaddr, char sq);
    int sockfd_;
    sockaddr_in address_;
    std::unordered_map<char, ClientElement> *clients_; //seq |-> sockaddr
    char seq_num_;
    std::mutex server_mutex_;
};

#endif