#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

int main() {
    int clientSocket;
    char buffer[1024];
    struct sockaddr_in serverAddr;
    socklen_t serverAddrSize = sizeof(serverAddr);

    clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket < 0) {
        cerr << "Error: Socket creation failed." << endl;
        return 1;
    }
    cout << "UDP Client Socket Created." << endl;

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8081);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Send dynamic message
    string message;
    cout << "Your message: ";
    getline(cin, message);
    sendto(clientSocket, message.c_str(), message.size(), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    // Receive reply
    memset(buffer, 0, sizeof(buffer));
    recvfrom(clientSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&serverAddr, &serverAddrSize);
    cout << "Server: " << buffer << endl;

    close(clientSocket);
    return 0;
}