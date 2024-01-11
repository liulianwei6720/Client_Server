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
#include <assert.h>

#include "signal.h"
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

Client::Client(void) : running_(true), connect_state_(false), ip_(nullptr), sockfd_(-1), port_(-1), buffer_size_(1024)
{
    buffer_ = (char *)malloc(buffer_size_);
    return;
}

Client::~Client(void)
{
    free(buffer_);
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
    
    choice = (Choice)(getchar() - '0');
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
            cout << "Sent failed!\n";
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

    thread listener(&Client::ListenTo, this);
    listener.detach();
    connect_state_ = true;
    return true;
}

void Client::ListenTo(void)
{
    memset(buffer_, 0, 256);
    while(true)
    {
        if(!connect_state_)
            break;
        ssize_t bytes = recv(sockfd_, buffer_, 256, 0);
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
                    while(buffer_[--tail] == 0);
                    for(int i = 0; i <= tail; i++)
                        putchar(buffer_[i]);
                    break;
                }
                else if(yn == "no" || yn == "n")
                    break;
                cout << "Input yes/no.\n";
            }

        }
        memset(buffer_, 0, 256);
    }
    return;
}

bool Client::Disconnect(void)
{
    if(connect_state_ == true)
        close(sockfd_);
    return true;
}


bool Client::GetList(void)
{// 接受数据写入map
    char gl = kClient;
    if(send(sockfd_, &gl, 1, 0) == -1)
        return false;
    // TODO write to clt_list_...    
    return true;
}



bool Client::SendTo(void)
{
    if(!GetList())
    {
        cout << "Get client list error.\n";
        return false;
    }
    cout << "Input the sequence number of client you want to send message to: (q to quit)";
    PrintCltList();
    while(true)
    {
        string in;
        cin >> in;
        if(in.empty())
        {
            cout << "Input a number or quit.\n";
            continue;
        }
        char *err_ptr = 0;
        char num = strtol(in.c_str(), &err_ptr, 10);
        if(*err_ptr != 0)
        {
            if(in == "q")
                return false;
            cout << "Input a valid number!\n";
        }
        else
            break;
    }
    // send signal to server.
    char sd = kSend;
    send(sockfd_, &sd, 1, 0);
    
    char message[1024];
    int msg_ptr = 0;
    memset(message, 0, 1024);
    cout << "Input the message you want to send to: (no more than 1k characters, input 2 enters to end input)\n";
    char state = 0;
    while(state != 2)
    {
        char ch = getchar();
        if(ch == '\n')
            state++;
        else
            state = 0;
        message[msg_ptr++] = ch;
    }
    ssize_t msg_size = strlen(message);
    if(msg_size != send(sockfd_, message, msg_size, 0))
    {
        cout << "Send error in Client.\n";
        return false;
    }
    return true;
}

bool Client::GetTime(void)
{
    char gt = kTime;
    if(send(sockfd_, &gt, 1, 0) == -1)
        return false;
    return true;
}

bool Client::GetName(void)
{
    char gn = kName;
    if(send(sockfd_, &gn, 1, 0) == -1)
        return false;
    return true;
}

void Client::Deserialize(void)
{
    // TODO decode byte stream rcv from server and store to `clt_list_`
    return;
}

void Client::PrintCltList(void)
{
    // TODO print control list cache
    return;
}

int main(void)
{
    Client *clt = new Client();
    while(!clt->QuitProg())
        clt->Work();
    delete clt;
    return 0;
}