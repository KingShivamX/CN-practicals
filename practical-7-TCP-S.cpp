#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

int main() {
    int serverSocket, clientSocket;
    char buffer[1024];
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
    
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        cerr << "Error: Socket creation failed." << endl;
        return 1;
    }
    cout << "TCP Server Socket Created." << endl;
    
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Error: Bind failed." << endl;
        return 1;
    }
    
    if (listen(serverSocket, 5) < 0) {
        cerr << "Error: Listen failed." << endl;
        return 1;
    }
    cout << "Server listening... waiting for connections." << endl;

    // Loop to accept multiple clients
    while (true) {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
        if (clientSocket < 0) {
            cerr << "Error: Accept failed." << endl;
            continue; // Wait for next client
        }
        cout << "Client connected." << endl;

        // Receive message
        memset(buffer, 0, sizeof(buffer));
        recv(clientSocket, buffer, sizeof(buffer), 0);
        cout << "Client: " << buffer << endl;

        // Send dynamic reply
        string reply;
        cout << "Your reply: ";
        getline(cin, reply); 
        send(clientSocket, reply.c_str(), reply.size(), 0);
        cout << "Reply sent." << endl;

        close(clientSocket); // Close this client's socket
        cout << "Client disconnected. Waiting for new connection..." << endl;
    }

    close(serverSocket);
    return 0;
}