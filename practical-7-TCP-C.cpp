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

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        cerr << "Error: Socket creation failed." << end l;
        return 1;
    }
    
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Error: Connection to server failed." << endl;
        return 1;
    }
    cout << "Connected to server." << endl;

    // Send dynamic message
    string message;
    cout << "Your message: ";
    getline(cin, message);
    send(clientSocket, message.c_str(), message.size(), 0);

    // Receive reply
    memset(buffer, 0, sizeof(buffer));
    recv(clientSocket, buffer, sizeof(buffer), 0);
    cout << "Server: " << buffer << endl;

    close(clientSocket);
    return 0;
}