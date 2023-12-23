#include <iostream>
#include <cstring>
#include <cstdlib>
#include <thread>
#include <mutex>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "client.h"
using namespace std;

Client::Client(void) : state_(false), ip_(nullptr), sockfd_(-1), port_(-1)
{
    return;
}

void Client::Work(void)
{
    
}

int main(int argc, char **argv) 
{
    // 创建套接字
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    // 连接服务器
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddress.sin_port = htons(8080);
    connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    // 发送消息给服务器
    const char* message = "Hello World";
    send(clientSocket, message, strlen(message), 0);
    std::cout << "Sent message to server: " << message << std::endl;

    // 接收服务器的响应
    char buffer[256];
    recv(clientSocket, buffer, sizeof(buffer), 0);
    std::cout << "Received result from server: " << buffer << std::endl;

    // 关闭套接字
    close(clientSocket);

    return 0;
}