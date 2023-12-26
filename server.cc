#include <iostream>
#include <thread>
#include <unordered_map>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


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
