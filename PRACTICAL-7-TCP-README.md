## Practical 7 (TCP): Client–Server Chat – Theory & Full Code Explanation

This README explains **both TCP client and TCP server**:

-   Theory of **TCP**, sockets, and connection flow.
-   Then **line-by-line explanation** of:
    -   `practical-7-TCP-S.cpp` (server)
    -   `practical-7-TCP-C.cpp` (client)

All code is standard POSIX socket code (Linux-style).

---

### 1. TCP Theory – Quick Recap

-   **TCP (Transmission Control Protocol)**:

    -   **Connection-oriented**: a reliable connection must be established before data exchange.
    -   **Reliable**: guarantees delivery, correct order, and no duplication.
    -   **Stream-based**: treats data as a continuous stream of bytes (not fixed messages).
    -   Uses **3-way handshake**:
        1. Client → Server: `SYN`
        2. Server → Client: `SYN + ACK`
        3. Client → Server: `ACK`

-   **Roles**:

    -   **Server**:
        -   Creates a socket.
        -   Binds it to an IP and **port** (e.g. `8080`).
        -   Listens for incoming connections.
        -   Accepts a connection and communicates.
    -   **Client**:
        -   Creates a socket.
        -   Connects to the server’s IP and port.
        -   Sends and receives data.

-   **System Calls / Functions** used:
    -   `socket()` – create a socket.
    -   `bind()` – attach socket to a local address (IP + port).
    -   `listen()` – mark socket as passive (server, waiting for clients).
    -   `accept()` – accept a connection from a client.
    -   `connect()` – client connects to server.
    -   `send()` / `recv()` – send/receive data over a TCP connection.
    -   `close()` – close the socket.

Your TCP programs implement **a simple one-message chat**:

-   Client:
    -   Connects, sends one message, receives one reply, closes.
-   Server:
    -   Waits for clients in a loop.
    -   For each client: receives message, asks user for reply, sends it, closes client socket, repeats.

---

## 2. TCP SERVER – `practical-7-TCP-S.cpp`

```1:9:practical-7-TCP-S.cpp
#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;
```

-   `#include <iostream>` – for `cout`, `cin`, `cerr`.
-   `#include <string>` – for `std::string`.
-   `#include <cstring>` – for `memset`.
-   `#include <sys/socket.h>` – core socket functions: `socket`, `bind`, `listen`, `accept`, `send`, `recv`.
-   `#include <netinet/in.h>` – `sockaddr_in`, `INADDR_ANY`, `htons`, etc.
-   `#include <arpa/inet.h>` – conversion between binary and text IP (`inet_addr` etc.).
-   `#include <unistd.h>` – `close` function.
-   `using namespace std;` – so you can write `cout` instead of `std::cout`.

```11:16:practical-7-TCP-S.cpp
int main() {
    int serverSocket, clientSocket;
    char buffer[1024];
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
```

-   `serverSocket`: listening socket (accepts connections).
-   `clientSocket`: socket for the connected client.
-   `buffer[1024]`: buffer for receiving client messages.
-   `serverAddr`: holds server’s address (port, IP, family).
-   `clientAddr`: will hold client’s address after `accept`.
-   `clientAddrSize`: size of `clientAddr` structure.

```17:22:practical-7-TCP-S.cpp
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        cerr << "Error: Socket creation failed." << endl;
        return 1;
    }
    cout << "TCP Server Socket Created." << endl;
```

-   `socket(AF_INET, SOCK_STREAM, 0)`:
    -   `AF_INET` – IPv4.
    -   `SOCK_STREAM` – TCP.
    -   `0` – default protocol for TCP.
-   If result < 0 → error.
-   On success, print confirmation.

```24:28:practical-7-TCP-S.cpp
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
```

-   `memset(&serverAddr, 0, ...)` – initialize struct to zero.
-   `sin_family = AF_INET` – IPv4.
-   `sin_port = htons(8080)`:
    -   `8080` is the port.
    -   `htons` converts from host byte order to network byte order (big-endian).
-   `sin_addr.s_addr = INADDR_ANY` – listen on **all local interfaces** (any local IP).

```29:32:practical-7-TCP-S.cpp
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Error: Bind failed." << endl;
        return 1;
    }
```

-   `bind()` attaches the socket to the address (IP + port) in `serverAddr`.
-   If it fails (e.g. port already in use) → print error and exit.

```34:37:practical-7-TCP-S.cpp
    if (listen(serverSocket, 5) < 0) {
        cerr << "Error: Listen failed." << endl;
        return 1;
    }
    cout << "Server listening... waiting for connections." << endl;
```

-   `listen(serverSocket, 5)`:
    -   Puts server socket in **passive** listening mode.
    -   `5` is the backlog (max pending connections in queue).
-   On success, prints that server is ready.

```40:47:practical-7-TCP-S.cpp
    // Loop to accept multiple clients
    while (true) {
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
        if (clientSocket < 0) {
            cerr << "Error: Accept failed." << endl;
            continue; // Wait for next client
        }
        cout << "Client connected." << endl;
```

-   Infinite loop: server can handle **multiple clients sequentially**.
-   `accept()`:
    -   Waits until a client connects.
    -   Returns a new socket (`clientSocket`) **for that connection only**.
-   If `accept` fails, print error and continue (try again).
-   On success, print that client is connected.

```49:53:practical-7-TCP-S.cpp
        // Receive message
        memset(buffer, 0, sizeof(buffer));
        recv(clientSocket, buffer, sizeof(buffer), 0);
        cout << "Client: " << buffer << endl;
```

-   Clear `buffer`.
-   `recv(clientSocket, buffer, sizeof(buffer), 0)`:
    -   Blocks until data from client arrives.
    -   Stores received data into `buffer`.
-   Prints the client’s message.

```55:59:practical-7-TCP-S.cpp
        // Send dynamic reply
        string reply;
        cout << "Your reply: ";
        getline(cin, reply);
        send(clientSocket, reply.c_str(), reply.size(), 0);
        cout << "Reply sent." << endl;
```

-   `reply` is a C++ `string`.
-   `getline(cin, reply)` reads a full line from user (server side).
    -   Note: If there is a leftover newline in the input buffer (from previous `cin`), it might need a `cin.ignore()` in a perfect program, but here it’s kept simple.
-   `send(clientSocket, reply.c_str(), reply.size(), 0)`:
    -   Sends raw bytes of the reply over the TCP connection.
-   Print confirmation.

```61:63:practical-7-TCP-S.cpp
        close(clientSocket); // Close this client's socket
        cout << "Client disconnected. Waiting for new connection..." << endl;
    }
```

-   Close the connection to this client.
-   Loop continues, server waits for next client via `accept()`.

```65:67:practical-7-TCP-S.cpp
    close(serverSocket);
    return 0;
}
```

-   (Not reached in this code because of `while (true)`, but correct cleanup if you ever break the loop.)

---

## 3. TCP CLIENT – `practical-7-TCP-C.cpp`

```1:9:practical-7-TCP-C.cpp
#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;
```

-   Same includes and `using namespace std;` as server (client also uses sockets, strings, IO).

```11:15:practical-7-TCP-C.cpp
int main() {
    int clientSocket;
    char buffer[1024];
    struct sockaddr_in serverAddr;
```

-   `clientSocket`: TCP socket used to connect to server.
-   `buffer`: for receiving server’s reply.
-   `serverAddr`: holds server IP/port info.

```16:20:practical-7-TCP-C.cpp
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        cerr << "Error: Socket creation failed." << endl;
        return 1;
    }
```

-   Create a TCP socket (same as server).
-   On failure, exit.

```22:25:practical-7-TCP-C.cpp
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
```

-   Clear `serverAddr`.
-   `AF_INET` – IPv4.
-   `htons(8080)` – server is listening on port 8080.
-   `inet_addr("127.0.0.1")`:
    -   Converts `"127.0.0.1"` (localhost) to binary IP for `sin_addr.s_addr`.
-   So this client connects to a **TCP server on localhost:8080**.

```27:31:practical-7-TCP-C.cpp
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Error: Connection to server failed." << endl;
        return 1;
    }
    cout << "Connected to server." << endl;
```

-   `connect()`:
    -   Asks the OS to establish a TCP connection to server’s address.
    -   If server isn't running or firewall blocks the port, this fails.
-   On success, prints confirmation.

```33:38:practical-7-TCP-C.cpp
    // Send dynamic message
    string message;
    cout << "Your message: ";
    getline(cin, message);
    send(clientSocket, message.c_str(), message.size(), 0);
```

-   Prompt user for a message.
-   `getline(cin, message)` reads entire line.
    -   If there was a previous `cin >>`, you might need `cin.ignore()` (not included here).
-   `send(clientSocket, message.c_str(), message.size(), 0)`:
    -   Sends this message to the server over the TCP connection.

```40:42:practical-7-TCP-C.cpp
    // Receive reply
    memset(buffer, 0, sizeof(buffer));
    recv(clientSocket, buffer, sizeof(buffer), 0);
    cout << "Server: " << buffer << endl;
```

-   Clear `buffer`.
-   `recv(clientSocket, buffer, sizeof(buffer), 0)`:
    -   Waits for server reply.
    -   Stores reply in `buffer`.
-   Print `"Server: <reply>"`.

```44:46:practical-7-TCP-C.cpp
    close(clientSocket);
    return 0;
}
```

-   Close connection and exit.

---

## 4. End-to-End Flow Summary (TCP)

### Server side:

1. Create TCP socket (`socket`).
2. Fill `serverAddr` with `AF_INET`, port 8080, `INADDR_ANY`.
3. `bind` socket to this address.
4. `listen` on socket (waiting for connections).
5. Loop:
    - `accept` a client → `clientSocket`.
    - `recv` one message from client.
    - Read user reply (from keyboard) into `reply`.
    - `send` reply back to client.
    - `close` client socket.

### Client side:

1. Create TCP socket (`socket`).
2. Fill `serverAddr` with `AF_INET`, port 8080, IP `127.0.0.1`.
3. `connect` to server.
4. Read message from user.
5. `send` message to server.
6. `recv` reply from server.
7. Print reply and `close` socket.

Together this forms a simple **TCP client–server chat** where:

-   TCP guarantees that the entire message is reliably delivered in order.
-   The code demonstrates the standard TCP programming pattern for practical exams.
