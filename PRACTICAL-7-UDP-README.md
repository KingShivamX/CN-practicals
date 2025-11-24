## Practical 7 (UDP): Client–Server Chat – Theory & Full Code Explanation

This README explains **both UDP client and UDP server**:
- Theory of **UDP**, datagrams, and flow.
- Then **line-by-line explanation** of:
  - `practical-7-UDP-S.cpp` (server)
  - `practical-7-UDP-C.cpp` (client)

---

### 1. UDP Theory – Quick Recap

- **UDP (User Datagram Protocol)**:
  - **Connectionless**: no connection setup (no 3-way handshake).
  - **Unreliable**: no guarantee of delivery, order, or duplication protection.
  - **Message-oriented**: sends **datagrams** (independent packets).
  - Lower overhead and latency than TCP, often used for:
    - DNS, VoIP, streaming, games, etc.

- **Roles**:
  - **Server**:
    - Creates a UDP socket.
    - Binds it to a local IP and port.
    - Waits for datagrams from any client.
  - **Client**:
    - Creates a UDP socket.
    - Sends datagrams to server’s IP and port using `sendto`.
    - Receives replies using `recvfrom`.

- **Key differences from TCP**:
  - No `listen()`, no `accept()`, no `connect()` required (though UDP can optionally use `connect`, not shown here).
  - Uses `sendto()` and `recvfrom()` instead of `send()` and `recv()`.
  - Server never creates a separate “client socket”; it just uses the same socket to talk to all clients.

Your UDP programs implement a simple **request–reply**:
- Client:
  - Sends one message to server.
  - Waits for a reply.
- Server:
  - Waits in a loop for datagrams.
  - For each incoming datagram: reads it, asks user for reply, sends reply back to that client.

---

## 2. UDP SERVER – `practical-7-UDP-S.cpp`

```1:9:practical-7-UDP-S.cpp
#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;
```

- `#include <iostream>` – for printing and input.
- `#include <string>` – for `std::string` replies.
- `#include <cstring>` – for `memset`.
- `#include <sys/socket.h>` – socket API.
- `#include <netinet/in.h>` – `sockaddr_in`, `htons`, etc.
- `#include <arpa/inet.h>` – IP address conversions.
- `#include <unistd.h>` – `close`.
- `using namespace std;` – use `cout`, `string` without `std::`.

```11:16:practical-7-UDP-S.cpp
int main() {
    int serverSocket;
    char buffer[1024];
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
```

- `serverSocket`: the UDP socket used to receive and send datagrams.
- `buffer[1024]`: storage for incoming messages.
- `serverAddr`: server’s local address (port, IP).
- `clientAddr`: address of the **client** that sent the current datagram.
- `clientAddrSize`: size of `clientAddr` struct.

```17:23:practical-7-UDP-S.cpp
    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket < 0) {
        cerr << "Error: Socket creation failed." << endl;
        return 1;
    }
    cout << "UDP Server Socket Created." << endl;
```

- `socket(AF_INET, SOCK_DGRAM, 0)`:
  - `AF_INET` – IPv4.
  - `SOCK_DGRAM` – UDP (datagram).
- On error, print message and exit.
- On success, print confirmation.

```24:28:practical-7-UDP-S.cpp
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8081);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
```

- Initialize `serverAddr` to zero.
- `AF_INET` – IPv4.
- Port: `8081`, converted to network byte order with `htons`.
- IP: `INADDR_ANY` – bind to all local interfaces.

```29:33:practical-7-UDP-S.cpp
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Error: Bind failed." << endl;
        return 1;
    }
    cout << "Bind successful to port 8081." << endl;
    cout << "Server waiting for datagram..." << endl;
```

- `bind()` associates this UDP socket with port 8081 (and any local IP).
- On success:
  - Server is ready to receive datagrams on `port 8081`.

```36:38:practical-7-UDP-S.cpp
    // Loop to handle multiple datagrams
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        recvfrom(serverSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientAddr, &clientAddrSize);
        cout << "Client: " << buffer << endl;
```

- Infinite loop: server can handle **many datagrams** from any clients.
- `memset(buffer, 0, ...)` – clear buffer before each receive.
- `recvfrom(...)`:
  - Waits for a UDP datagram.
  - Fills `buffer` with received data.
  - Fills `clientAddr` with sender’s IP and port.
- Print the received message: `"Client: <message>"`.

```42:47:practical-7-UDP-S.cpp
        // Send dynamic reply
        string reply;
        cout << "Your reply: ";
        getline(cin, reply);
        sendto(serverSocket, reply.c_str(), reply.size(), 0, (struct sockaddr*)&clientAddr, clientAddrSize);
        cout << "Reply sent to client." << endl;
    }
```

- `reply` is built interactively from the server’s console.
- `getline(cin, reply)` reads a full line.
  - Same note as TCP: if there was a prior `cin >>`, you’d normally call `cin.ignore()` first.
- `sendto(...)`:
  - Sends reply bytes to the **client address** stored in `clientAddr`.
  - No separate client socket; just re-use `serverSocket`.
- The `while (true)` continues: server remains ready for more datagrams.

```50:52:practical-7-UDP-S.cpp
    close(serverSocket);
    return 0;
}
```

- (Not reached due to infinite loop but correct cleanup if loop ended.)

---

## 3. UDP CLIENT – `practical-7-UDP-C.cpp`

```1:9:practical-7-UDP-C.cpp
#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;
```

- Same headers and namespace as the server; client also uses sockets, strings, IO.

```11:16:practical-7-UDP-C.cpp
int main() {
    int clientSocket;
    char buffer[1024];
    struct sockaddr_in serverAddr;
    socklen_t serverAddrSize = sizeof(serverAddr);
```

- `clientSocket`: UDP socket for sending/receiving.
- `buffer`: to hold server’s reply.
- `serverAddr`: server’s IP/port.
- `serverAddrSize`: size of `serverAddr`.

```17:23:practical-7-UDP-C.cpp
    clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSocket < 0) {
        cerr << "Error: Socket creation failed." << endl;
        return 1;
    }
    cout << "UDP Client Socket Created." << endl;
```

- `socket(AF_INET, SOCK_DGRAM, 0)`:
  - Create a UDP socket.
- Check for errors and print success message.

```24:27:practical-7-UDP-C.cpp
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8081);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
```

- Clear `serverAddr`.
- `AF_INET` – IPv4.
- Port: 8081 (same as server).
- IP: `"127.0.0.1"` → localhost (same machine).
- So the client targets UDP server at **127.0.0.1:8081**.

```29:33:practical-7-UDP-C.cpp
    // Send dynamic message
    string message;
    cout << "Your message: ";
    getline(cin, message);
    sendto(clientSocket, message.c_str(), message.size(), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
```

- Prompt user for message.
- Read it using `getline`.
- `sendto(...)`:
  - Sends the message datagram to the server’s address (`serverAddr`).
  - No need to call `connect()`; UDP is connectionless.

```35:38:practical-7-UDP-C.cpp
    // Receive reply
    memset(buffer, 0, sizeof(buffer));
    recvfrom(clientSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&serverAddr, &serverAddrSize);
    cout << "Server: " << buffer << endl;
```

- Clear `buffer`.
- `recvfrom(...)`:
  - Waits for a reply datagram.
  - Receives data into `buffer`.
  - Updates `serverAddr`/`serverAddrSize` with sender address (the server).
- Prints the server’s reply.

```40:42:practical-7-UDP-C.cpp
    close(clientSocket);
    return 0;
}
```

- Close UDP socket and exit.

---

## 4. End-to-End Flow Summary (UDP)

### Server side:
1. Create UDP socket (`socket(AF_INET, SOCK_DGRAM, 0)`).
2. Fill `serverAddr` with `AF_INET`, port 8081, `INADDR_ANY`.
3. `bind` socket to this address.
4. Loop:
   - `recvfrom` to receive a datagram into `buffer`, along with client address `clientAddr`.
   - Print client’s message.
   - Read reply from keyboard into `reply`.
   - `sendto` reply back to that `clientAddr`.

### Client side:
1. Create UDP socket.
2. Fill `serverAddr` with `AF_INET`, port 8081, IP `127.0.0.1`.
3. Read message from user.
4. `sendto` message to server.
5. `recvfrom` reply from server.
6. Print reply and close socket.

---

## 5. TCP vs UDP in Your Practicals (High-Level)

- **TCP (in TCP README)**:
  - Uses `SOCK_STREAM`, `listen`, `accept`, `connect`, `send`, `recv`.
  - Connection-based, reliable stream.
  - Server creates a new `clientSocket` for each connection.

- **UDP (this README)**:
  - Uses `SOCK_DGRAM`, `bind`, `sendto`, `recvfrom`.
  - No connections; just datagrams.
  - Server uses **one socket** for all clients, distinguished by `clientAddr`.

Both sets of programs implement a **simple chat** pattern that demonstrates the typical exam flow for TCP and UDP client–server sockets.


