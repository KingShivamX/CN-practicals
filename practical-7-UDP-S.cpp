#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

int main() {
    int serverSocket;
    char buffer[1024];
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);

    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket < 0) {
        cerr << "Error: Socket creation failed." << endl;
        return 1;
    }
    cout << "UDP Server Socket Created." << endl;

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8081);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Error: Bind failed." << endl;
        return 1;
    }
    cout << "Bind successful to port 8081." << endl;
    cout << "Server waiting for datagram..." << endl;

    // Loop to handle multiple datagrams
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        recvfrom(serverSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientAddr, &clientAddrSize);
        cout << "Client: " << buffer << endl;

        // Send dynamic reply
        string reply;
        cout << "Your reply: ";
        getline(cin, reply);
        sendto(serverSocket, reply.c_str(), reply.size(), 0, (struct sockaddr*)&clientAddr, clientAddrSize);
        cout << "Reply sent to client." << endl;
    }

    close(serverSocket);
    return 0;
}