#ifndef CLIENT_H
#define CLIENT_H

#include <unordered_set>
#include <arpa/inet.h>
#include <netinet/in.h>
class Client
{
public:
    Client();
    void Work(void);

private:
    void set_runnning(bool state) { running_ = runnning; };
    bool runnning(void) const { return running_; }
    bool Connect(void);
    bool Disconnect(void);
    bool GetTime(void);
    bool GetName(void);
    bool GetList(void);
    bool SendTo(void);
    void Quit(void);
    void Prompt(void);
    std::unordered_set<char *> clt_list_;
    bool running_;
    bool connect_state_;
    const char *ip_;
    int sockfd_;
    int port_;
    sockaddr_in addr;
};


#endif