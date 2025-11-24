#include <iostream>
#include <thread>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <algorithm>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

unordered_map< string, int> users; // username -> socket
mutex mtx;

void broadcastMessage(const  string &msg, int senderSocket) {
    lock_guard< mutex> lock(mtx);
    for (auto &p : users) {
        if (p.second != senderSocket) {
            send(p.second, msg.c_str(), msg.size(), 0);
        }
    }
}

void handleClient(int clientSocket) {
    char buffer[1024];
    string username;

    // Step 1: Get username
    send(clientSocket, "Enter username: ", 16, 0);
    memset(buffer, 0, sizeof(buffer));
    recv(clientSocket, buffer, sizeof(buffer), 0);
    username = buffer;
    username.erase( remove(username.begin(), username.end(), '\n'), username.end());
    username.erase( remove(username.begin(), username.end(), '\r'), username.end());

    {
        lock_guard< mutex> lock(mtx);
        users[username] = clientSocket;
    }

    string joinMsg = username + " joined the chat.\n";
    cout << joinMsg;
    broadcastMessage(joinMsg, clientSocket);

    // Step 2: Handle chat
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytes <= 0) {
             cout << username << " disconnected.\n";
            close(clientSocket);
            {
                lock_guard< mutex> lock(mtx);
                users.erase(username);
            }
            string leaveMsg = username + " left the chat.\n";
            broadcastMessage(leaveMsg, -1);
            break;
        }

        string msg(buffer);
        msg.erase( remove(msg.begin(), msg.end(), '\n'), msg.end());
        msg.erase( remove(msg.begin(), msg.end(), '\r'), msg.end());

        // Step 3: Command parsing
        if (msg.rfind("/msg", 0) == 0) {
            istringstream iss(msg);
            string cmd, target;
            iss >> cmd >> target;
            string text;
            getline(iss, text);
            if (!text.empty() && text[0] == ' ') text.erase(0, 1);

            lock_guard< mutex> lock(mtx);
            if (users.count(target)) {
                string fullMsg = "[PM from " + username + "]: " + text + "\n";
                send(users[target], fullMsg.c_str(), fullMsg.size(), 0);
            } else {
                string err = "User '" + target + "' not found.\n";
                send(clientSocket, err.c_str(), err.size(), 0);
            }
        } else {
             string fullMsg = "[" + username + "]: " + msg + "\n";
            broadcastMessage(fullMsg, clientSocket);
        }
    }
}

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 5);
    cout << "Server started on port 8080...\n";

    while (true) {
        sockaddr_in clientAddr{};
        socklen_t clientSize = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);
        thread(handleClient, clientSocket).detach();
    }

    close(serverSocket);
    return 0;
}