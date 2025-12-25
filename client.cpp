// client.cpp
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <chrono> // For high-precision timing
#include "matrix_utils.h"

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define MATRIX_SIZE 500 // N size

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    std::cout << "--- Distributed Matrix Client ---" << std::endl;
    std::cout << "Target Size: " << MATRIX_SIZE << "x" << MATRIX_SIZE << std::endl;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return -1;
    }

    std::cout << "Connecting to server at " << SERVER_IP << "..." << std::endl;
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return -1;
    }
    std::cout << "Connected!" << std::endl;

    std::cout << "Generating random matrices..." << std::endl;
    Matrix A(MATRIX_SIZE, MATRIX_SIZE);
    Matrix B(MATRIX_SIZE, MATRIX_SIZE);
    randomizeMatrix(A);
    randomizeMatrix(B);

    auto start_time = std::chrono::high_resolution_clock::now();

    try {
        std::cout << "Sending Matrix A (" << (A.rows * A.cols * sizeof(int))/1024 << " KB)..." << std::endl;
        sendMatrix(sock, A);

        std::cout << "Sending Matrix B (" << (B.rows * B.cols * sizeof(int))/1024 << " KB)..." << std::endl;
        sendMatrix(sock, B);

        std::cout << "Waiting for result from server..." << std::endl;
        Matrix C = recvMatrix(sock);
      
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end_time - start_time;

        std::cout << "Result received!" << std::endl;
        std::cout << "Result Dimensions: " << C.rows << "x" << C.cols << std::endl;
        std::cout << "Total Round-Trip Time: " << elapsed.count() << " seconds" << std::endl;

        std::cout << "Top-left corner value: " << C(0,0) << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Communication error: " << e.what() << std::endl;
    }

    // 9. Close Socket
    close(sock);
    return 0;
}
