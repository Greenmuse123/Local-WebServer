#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <cstring>

#pragma comment(lib, "ws2_32.lib") // Link against the Winsock library

int main() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return 1;
    }

    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        std::cerr << "Socket creation failed with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Use any available interface
    address.sin_port = htons(8080); // Port to listen on

    if (bind(server_fd, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == SOCKET_ERROR) {
        std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    if (listen(server_fd, 10) == SOCKET_ERROR) {
        std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    std::cout << "+++++++ Waiting for new connection ++++++++\n" << std::endl;

    SOCKET new_socket;
    sockaddr_in client_address;
    int addrlen = sizeof(client_address);

    while ((new_socket = accept(server_fd, reinterpret_cast<sockaddr*>(&client_address), &addrlen)) != INVALID_SOCKET) {
        char buffer[30000] = {0};
        int valread = recv(new_socket, buffer, sizeof(buffer), 0);
        if (valread == SOCKET_ERROR) {
            std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
        } else {
            std::cout << buffer << std::endl; // Printing out the request to the console
            const char* hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\n\nHello, World!";
            send(new_socket, hello, static_cast<int>(strlen(hello)), 0);
            std::cout << "------------------Hello message sent-------------------\n";
        }
        closesocket(new_socket);
    }

    if (new_socket == INVALID_SOCKET) {
        std::cerr << "Accept failed with error: " << WSAGetLastError() << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    closesocket(server_fd);
    WSACleanup(); // Clean up Winsock
    return 0;
}
