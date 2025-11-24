/* "Include" lines are like telling the program
   "We need to use tools from these toolboxes." */
#include <iostream>     // For printing to console (cout) and reading keyboard (cin, getline)
#include <string>       // For using the "string" variable type (for our dynamic reply)
#include <cstring>      // For "memset", a function to wipe memory clean
#include <sys/socket.h> // The main toolbox for all socket functions
#include <netinet/in.h> // For internet address structures (like sockaddr_in)
#include <arpa/inet.h>  // For functions like 'inet_addr' (not used here, but common)
#include <unistd.h>     // For the "close" function (to hang up connections)

// This saves us from typing "std::" before cout, cin, string, etc.
using namespace std;

// The 'main' function is where your program always starts
int main() {
    /* --- 1. DEFINE VARIABLES --- */

    // These are like ID numbers for our connections.
    // 'serverSocket' is our own "phone line" for listening.
    // 'clientSocket' is the specific "call" we are currently on.
    int serverSocket, clientSocket;
    
    // 'buffer' is a "box" or "storage area" (an array of 1024 characters)
    // We use this box to hold the message we receive from the client.
    char buffer[1024];

    // These are "address book" structures.
    // 'serverAddr' will hold OUR (the server's) address and port.
    // 'clientAddr' will be filled with the CLIENT's address and port when they connect.
    struct sockaddr_in serverAddr, clientAddr;
    
    // A variable to hold the size of the client's "address book" structure.
    // The 'accept' function needs this.
    socklen_t clientAddrSize = sizeof(clientAddr);
    
    /* --- 2. CREATE THE SERVER SOCKET --- */
    
    // 'socket()' is the function to create a new socket.
    // It gives us back the ID number (serverSocket).
    // AF_INET: "Address Family - Internet". This means we are using IPv4 (e.g., 127.0.0.1).
    // SOCK_STREAM: "Stream Socket". This means we want to use TCP (a reliable,
    //              connection-based protocol, like a phone call).
    // 0: The default protocol (which is automatically TCP when you use SOCK_STREAM).
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) { // If the ID is -1, it means creation failed.
        cerr << "Error: Socket creation failed." << endl;
        return 1; // Exit the program with an error.
    }
    cout << "TCP Server Socket Created." << endl;

    /* --- 3. CONFIGURE AND BIND THE SERVER ADDRESS --- */

    // 'memset' is used to "wipe the slate clean". It fills the 'serverAddr'
    // "address book" with zeros so there's no random garbage data in it.
    memset(&serverAddr, 0, sizeof(serverAddr));
    
    // Now we fill in our address book...
    serverAddr.sin_family = AF_INET; // We are using IPv4
    serverAddr.sin_port = htons(8080); // We set our "door number" (port) to 8080.
                                       // 'htons' ("Host to Network Short") converts the
                                       // number to a format all computers understand.
    serverAddr.sin_addr.s_addr = INADDR_ANY; // We will accept connections on ANY of
                                             // this computer's IP addresses.

    // 'bind()' is the function that "assigns" our address (serverAddr) to our
    // socket (serverSocket). Now the computer knows that any data for
    // port 8080 should go to this program.
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Error: Bind failed." << endl; // Fails if port 8080 is already in use
        return 1;
    }
    cout << "Bind successful to port 8080." << endl;

    /* --- 4. LISTEN FOR CONNECTIONS --- */

    // 'listen()' "turns on the ringer" for our socket. It tells the
    // Operating System to start accepting incoming connections.
    // '5' is the "backlog" - how many clients can wait in a queue if we are busy.
    if (listen(serverSocket, 5) < 0) {
        cerr << "Error: Listen failed." << endl;
        return 1;
    }
    cout << "Server listening... waiting for connections." << endl;

    /* --- 5. ACCEPT AND HANDLE CLIENTS (IN A LOOP) --- */

    // This is an infinite loop. It makes the server run forever,
    // so it can help one client, and then loop back to wait for the next one.
    // This is the simplest way to handle "more than 1" client (one after another).
    while (true) {
        
        // 'accept()' "answers the phone." This function WAITS (blocks)
        // until a client tries to connect.
        // When a client connects, 'accept' creates a NEW socket (clientSocket)
        // just for talking to *that specific client*.
        // It also fills in 'clientAddr' with the client's "caller ID" (their address).
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
        if (clientSocket < 0) {
            cerr << "Error: Accept failed." << endl;
            continue; // 'continue' skips this client and goes back to the start of the loop
        }
        cout << "Client connected." << endl;

        /* --- 6. COMMUNICATE WITH THE CONNECTED CLIENT --- */
        
        // Empty the "box" (buffer) before we put new data in it.
        memset(buffer, 0, sizeof(buffer));
        
        // 'recv()' (receive) WAITS for the client to send data.
        // It reads data from the 'clientSocket' and stores it in our 'buffer'.
        recv(clientSocket, buffer, sizeof(buffer), 0);
        
        // Print the message we just received.
        cout << "Client: " << buffer << endl;

        // --- This is the new DYNAMIC part ---
        // Create a C++ string to hold our reply.
        string reply;
        // Ask the server user (you) to type a reply.
        cout << "Your reply: ";
        // 'getline' reads a whole line from the keyboard (cin)
        // and stores it in the 'reply' string.
        getline(cin, reply); 
        // --- End of dynamic part ---

        // 'send()' sends data *to* the client.
        // We send the data from our 'reply' string.
        // '.c_str()' converts the C++ string into the
        // format that the 'send' function understands.
        send(clientSocket, reply.c_str(), reply.size(), 0);
        cout << "Reply sent." << endl;

        /* --- 7. CLOSE THE CLIENT SOCKET --- */
        
        // 'close()' "hangs up" on the *specific client*.
        // This is very important! It frees up resources.
        // The 'serverSocket' is STILL open and listening.
        close(clientSocket);
        cout << "Client disconnected. Waiting for new connection..." << endl;
        
    } // The 'while(true)' loop repeats, going back to 'accept()'

    // This line is never reached in this simple code, but if we
    // broke the loop, we would close the main server socket.
    close(serverSocket);
    return 0;
}