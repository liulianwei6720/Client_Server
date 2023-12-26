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

enum Choice {
    kConnect = 1,
    kDisConnect, 
    kGetTime,
    kGetName,
    kGetList,
    kSendTo,
    kQuit
};

Client::Client(void) : running_(true), connect_state_(false), ip_(nullptr), sockfd_(-1), port_(-1)
{
    return;
}

void Client::Prompt(void)
{
    cout << "Input number of the choice below:\n";
    cout << "1. Connect to a server with ipv4 address and port.\n";
    cout << "2. Disconnect from a server.\n";
    cout << "3. Get the time from server.\n";
    cout << "4. Get the other clients that connected to the server.\n";
    cout << "5. Send message to another client connected to the server.\n";
    cout << "6. Quit the program.\n";
    return;
}

void Client::Quit(void)
{
    if(connect_state_)
        Disconnect();
    set_runnning(false);
    return;
}

void Client::Work(void)
{
    Prompt();
    Choice choice;
    
    choice = (Choice)getchar();
    switch(choice)
    {
    case kConnect:
        if(!this->Connect())
            cout << "Connect failed.\n";
        else
            cout << "Connected successfully.\n";
        break;
    case kDisConnect:
        if(!Disconnect())
        {
            cout << "Internal error!\n";
            Quit();
        }
        else
            cout << "Disconnect successfully.\n";
        break;
    case kGetTime:
        if(!GetTime())
        {
            cout << "Internal error!\n";
            Quit();
        }
        else
            cout << "Sent...\n";
        break;
    case kGetName:
        if(!GetName())
        {
            cout << "Internal error!\n";
            Quit();
        }
        else
            cout << "Request Sent...\n";
        break;
    case kGetList:
        if(!GetList())
        {
            cout << "Internal error!\n";
            Quit();
        }
        else
            cout << "Request Sent...\n";
        break;
    case kSendTo:
        if(!SendTo())
        {
            cout << "Internal error!\n";
            Quit();
        }
        else
            cout << "Request Sent...\n";
        break;
    default:
        cout << "Input wrong number!" << endl;
    }
    return;
}

bool Client::Connect(void)
{
    sockfd_ = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    cout << "Input the ip address of server you want to connect to.\n";
    string s_ip;
    cin >> s_ip;
    if(0 > inet_pton(AF_INET, s_ip.c_str(), &addr.sin_addr.s_addr))
    {
        cout << "Ip incorrect!\n";
        return false;
    }
    cout << "Input the port number of server you want to connect to.\n";
    s_ip.clear();
    cin >> s_ip;
    port_ = htons(atoi(s_ip.c_str()));
    if(!connect(sockfd_, (sockaddr *)&addr, sizeof(addr)))
    {
        cout << "Connect error with the errcode: " << errno << endl;
        return false;
    }

    thread listener(&ListenTo, this);
    listener.detach();
    return true;
}

void Client::ListenTo(void)
{
    char *buffer = (char *)malloc(256);
    memset(buffer, 0, 256);
    while(true)
    {
        ssize_t bytes = recv(sockfd_, buffer, 256, 0);
        switch(bytes)
        {
        case -1:
            cout << "Receive error with code: " << errno << endl;
            break;
        case 0:
            cout << "Connect is closed.\n";
            break;
        default:
            cout << "Receive from server, print it?(yes/no)\n";
            string yn;
            while(true)
            {
                cin >> yn;
                if(yn == "yes" || yn == "y")
                {
                    int tail = 256;
                    while(buffer[--tail] == 0);
                    for(int i = 0; i <= tail; i++)
                        putchar(buffer[i]);
                    break;
                }
                else if(yn == "no" || yn == "n")
                    break;
                cout << "Input yes/no.\n";
            }

        }
        memset(buffer, 0, 256);
    }

    free(buffer);
    return;
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