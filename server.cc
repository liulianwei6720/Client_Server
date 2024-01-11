#include <iostream>
#include <cstdlib>
#include <unordered_map>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>

#include "server.h"
#include "signal.h"

using namespace std;

Server::Server(void) : seq_num_(0)
{
    clients_ = new unordered_map<char, ClientElement> ();
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    address_.sin_family = AF_INET;
    address_.sin_addr.s_addr = inet_addr("127.0.0.1");
    address_.sin_port = htons(5314);

    bind(sockfd_, (sockaddr *)&address_, sizeof(address_));

    listen(sockfd_, 10);
    return;
}

Server::~Server(void)
{
    delete clients_;
    return;
}

void Server::Work(void)
{
    sockaddr_in client_socket;
    socklen_t client_len = sizeof(client_socket);
    while(true)
    {
        memset(&client_socket, 0, sizeof(client_socket));
        int skt = accept(sockfd_, (sockaddr *)&client_socket, &client_len);
        server_mutex_.lock();
        clients_->insert(make_pair(++seq_num_, ClientElement(skt, client_socket)));
        server_mutex_.unlock();
        thread serve(&Server::ListenToClient, this, skt);
        serve.detach();
    }
    return;
}

void Server::ListenToClient(int skt)
{
    char *buf = (char *)malloc(1024);
    memset(buf, 0, 1024);
    sprintf(buf, "Hello!\n");
    ssize_t num_bytes = send(skt, buf, 1024, 0);
    if(num_bytes < 0)
    {
        cout << "Fatal error with errcode: " << errno << endl;
        exit(0);
    }
    memset(buf, 0, 1024);
    char sig = 0;
    time_t current_time;
    while(true)
    {// recv signal.
        num_bytes = recv(skt, &sig, 1, 0);
        if(num_bytes < 0)
        {
            cout << "Fatal error with errcode: " << errno << endl;
            exit(0);
        }   
        assert(num_bytes == 1);
        switch(sig)
        {
        case kTime:
            current_time = time(nullptr);
            sprintf(buf, "%s", ctime(&current_time));
            num_bytes = send(skt, buf, 1024, strlen(buf));
            if(num_bytes < 0)
                cout << "Send time error with errcode: " << errno << endl;
            break;   
        case kName: 
            if(!GetComputerName(buf, 1024))
                cout << "Get host name error with code: " << errno << endl;
            num_bytes = send(skt, buf, 1024, strlen(buf));
            if(num_bytes < 0)
                cout << "Send name error with code: " << errno << endl;
            break;
        case kClient:
            if(!GetClients(buf, 1024))
                cout << "Internal error!\n";
            num_bytes = send(skt, buf, 1024, 0);
            if(num_bytes < 0)
                cout << "Send clients error with code: " << errno << endl;
            break;
        case kSend:
            send(skt, "Give me the seq number.", strlen("Give me the seq number."), 0);
            num_bytes = recv(skt, buf, 1024, 0);
            break;
        default:
            cout << "Signal incorrect.\n";
            break;
        }
        memset(buf, 0, 1024);
    }
}



// seq ip port end
// 1   4   2    1
void Server::Serialize(size_t ptr, char *dst, size_t dst_size, sockaddr_in &skaddr, char seq)
{
    dst[ptr++] = seq;
    dst[ptr] = skaddr.sin_addr.s_addr;
    ptr += 4;
    dst[ptr] = skaddr.sin_port;
    ptr += 2;
    dst[ptr] = 0;
    return;
}

bool Server::GetClients(char *buf, size_t buf_size)
{
    size_t ptr = 0;
    size_t num_clients = 0;
    for(auto client = clients_->begin(); client != clients_->end(); clients_++)
    {
        if(ptr + 8 < buf_size)
            Serialize(ptr, buf, buf_size, client->second.addr, client->first);
        else
        {
            cout << "Not enough buffer size.\n";
            return false;
        }
        ptr += 8;
        num_clients++;
    }
    return true;
}

void Server::SendTo(int skt)
{
    return;
}

void Server::DeleteClient(int skt)
{
    return;
}

bool Server::GetComputerName(char *buf, size_t buf_size)
{
    return !gethostname(buf, buf_size);  
}

// listen on port 5314
int main(int argc, char **argv) 
{
    auto svr = new Server();
    svr->Work();
    return 0;
}
