#ifndef CLIENT_H
#define CLIENT_H

#include <unordered_map>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "signal.h"

class Client
{
public:
    Client();
    ~Client();
    void Work(void);

private:
    void set_runnning(bool state) { running_ = state; };
    bool runnning(void) const { return running_; }
    bool Connect(void);
    bool Disconnect(void);
    bool GetTime(void);
    bool GetName(void);
    bool GetList(void);
    bool SendTo(void);
    void Quit(void);
    void Prompt(void);
    void ListenTo(void);
    void Deserialize(void);
    void PrintCltList(void);
    std::unordered_map<char, sockaddr_in> clt_list_;
    
    mutex client_mutex_;
    char *buffer_;
    const size_t buffer_size_;
    bool running_;
    bool connect_state_;
    const char *ip_;
    int sockfd_; // connect to server.
    int port_;
    sockaddr_in addr;
};


#endif