//** This code creates a simple multithreaded web server that listens on port 8080 and 
//** responds with "Hello, World!" to any HTTP request. 
//*  The use of minimal STL and direct implementation of threading and networking 
//* functionalities make it more self-contained. 

#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> // Includes definitions for internet address family
#include <unistd.h>
#include <pthread.h> //ncludes the POSIX threads library

const int PORT = 8080;
const int BACKLOG = 10;
const int BUFFER_SIZE = 1024;

// Function to handle client requests 
// 
void* handleClientConnection(void *clientSocketPtr) {
    int clientSocket = *(int*)clientSocketPtr; // Dereference the pointer to get the value
    delete (int*)clientSocketPtr;  // Free the allocated memory for the pointer

    char buffer[BUFFER_SIZE];
    while (true) {
        // Receive data from the client
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
        if (bytesReceived <= 0) { //Checks if no data was received or an error occurred.Close the socket.
            close(clientSocket);
            break;
        }

        // Null-terminate the buffer
        buffer[bytesReceived] = '\0'; //serverSocket Null-terminates the received data to make it a valid C-string

        // Process the request
        std::string response = "HTTP/1.1 200 OK\r\n"; // Creates a simple HTTP response.
        response += "Content-Type: text/html\r\n\r\n"; // Adds the content type header to the response.
        response += "<html><body><h1>Hi,This is the era of Artificial Intelligence!</h1></body></html>"; 
        // Adds the HTML content to the response.

        // Send response back to the client
        send(clientSocket, response.c_str(), response.size(), 0); //  Pointer to the response data.
    }
    return nullptr;  // Returns nullptr to indicate the thread has finished execution.
}

// Main function
int main() {
    // Create a socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0); // Creates a new TCP socket , IPV4, 
    if (serverSocket == -1) {  // Checks if the socket creation failed.
        std::cerr << "Could not create socket\n"; // print errors 
        return 1; //  Exits the program with an error code.
    }

    // Set up the server address structure
    sockaddr_in serverAddr; // sockaddr_in Structure describing an Internet socket address 
    //Declares a sockaddr_in structure to hold the server address.
    serverAddr.sin_family = AF_INET; // set the address family to IPv4.
    serverAddr.sin_port = htons(PORT);  // Sets the port number 
    serverAddr.sin_addr.s_addr = INADDR_ANY;  //  Binds to any available network interface.

    memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));//Zeros out the rest of the structure 

    // Bind the socket to the address and port
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "Could not bind to port\n";
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, BACKLOG) == -1) {
        std::cerr << "Error listening on socket\n";
        return 1;
    }

    std::cout << "Server is listening on port " << PORT << "\n";

    while (true) {
        // Accept a new client connection
        sockaddr_in clientAddr;
        socklen_t clientSize = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientSize);
        if (clientSocket == -1) {
            std::cerr << "Error accepting connection\n";
            continue;
        }

        // Allocate memory for client socket pointer
        int* clientSocketPtr = new int(clientSocket);
        //and initializes it with the client socket descriptor.

        // Create a new thread to handle the client connection
        pthread_t thread;
        if (pthread_create(&thread, nullptr, handleClientConnection, clientSocketPtr) != 0) {
            std::cerr << "Error creating thread\n";
            delete clientSocketPtr;
            close(clientSocket);
        }

        // Detach the thread so it runs independently
        pthread_detach(thread);
    }

    // Close the server socket
    close(serverSocket);
    return 0;
}
