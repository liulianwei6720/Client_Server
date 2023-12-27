#include <iostream>
#include <cstdlib>
#include <thread>
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

Server::Server(void)
{
    clients_ = new unordered_map<int, sockaddr_in> ();
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
        clients_->insert(make_pair(skt, client_socket));
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
    while(true)
    {
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
            time_t current_time = time(nullptr);
            sprintf(buf, "%s", ctime(&current_time));
            num_bytes = send(skt, buf, 1024, strlen(buf));
            if(num_bytes < 0)
                cout << "Fatal error with errcode: " << errno << endl;
            break;   
        case kName: 
            if(!GetComputerName(buf, 1024))
                cout << "Get host name error with code: " << errno << endl;
            break;
        case kClient:

        case kSend:
        default:

        }
        memset(buf, 0, 1024);
    }
}

bool Server::GetClients(char *buf, size_t buf_size)
{
    for(auto client = clients_->begin(); client != clients_->end(); clients_++)
    {
        
    }
}

bool Server::GetComputerName(char *buf, size_t buf_size)
{
    return !gethostname(buf, buf_size);  
}

// listen on port 5314
int main(int argc, char **argv) 
{
    // 创建套接字
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // 绑定地址和端口
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8080);
    bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    // 监听连接
    listen(serverSocket, 5);

    std::cout << "Server is listening on port 8080..." << std::endl;

    // 接受连接
    int clientSocket = accept(serverSocket, nullptr, nullptr);
    std::cout << "Client connected." << std::endl;

    // 接收客户端的消息
    char buffer[256];
    recv(clientSocket, buffer, sizeof(buffer), 0);
    std::cout << "Received message from client: " << buffer << std::endl;

    // 处理消息并发送结果
    std::string result;
    const char* delimiter = " ";
    char* token = strtok(buffer, delimiter);
    while (token != nullptr) {
        result += token[0];
        token = strtok(nullptr, delimiter);
    }

    // 发送结果给客户端
    send(clientSocket, result.c_str(), result.size(), 0);
    std::cout << "Sent result to client: " << result << std::endl;

    // 关闭套接字
    close(clientSocket);
    close(serverSocket);

    return 0;
}
