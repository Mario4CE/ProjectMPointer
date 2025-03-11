#include "Server.h"

Servidor::Servidor(int port) : port(port), serverSocket(INVALID_SOCKET), running(false) {}

Servidor::~Servidor() {
    detener();
}

void Servidor::iniciar() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Error al inicializar Winsock." << std::endl;
        return;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Error al crear el socket." << std::endl;
        WSACleanup();
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Error al vincular el socket." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Error al escuchar en el socket." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    std::cout << "Servidor escuchando en el puerto " << port << std::endl;
    running = true;

    while (running) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Error al aceptar la conexión." << std::endl;
            continue;
        }

        std::thread clientThread(&Servidor::manejarCliente, this, clientSocket);
        clientThread.detach();
    }

    closesocket(serverSocket);
    WSACleanup();
}

void Servidor::detener() {
    running = false;
    closesocket(serverSocket);
}

void Servidor::manejarCliente(SOCKET clientSocket) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            break;
        }
        std::cout << "Mensaje recibido: " << buffer << std::endl;
        send(clientSocket, buffer, bytesReceived, 0);
    }
    closesocket(clientSocket);
}