// server.cpp
#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "matrix_utils.h" // Include our custom library

#define PORT 8080

Matrix multiplyMatrices(const Matrix& A, const Matrix& B) {
    if (A.cols != B.rows) {
        throw std::runtime_error("Matrix dimension mismatch for multiplication");
    }

    Matrix C(A.rows, B.cols);

    for (int i = 0; i < A.rows; ++i) {
        for (int j = 0; j < B.cols; ++j) {
            for (int k = 0; k < A.cols; ++k) {
                C(i, j) += A(i, k) * B(k, j);
            }
        }
    }
    return C;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    std::cout << "--- Distributed Matrix Multiplication Server ---" << std::endl;
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Listen on localhost
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server started. Listening on port " << PORT << "..." << std::endl;

    while (true) {
        std::cout << "\nWaiting for new client connection..." << std::endl;

        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }

        std::cout << "Client connected!" << std::endl;

        try {
            // 6. Receive Matrix A
            std::cout << "Receiving Matrix A..." << std::endl;
            Matrix A = recvMatrix(new_socket);
            std::cout << "Received A: " << A.rows << "x" << A.cols << std::endl;

            std::cout << "Receiving Matrix B..." << std::endl;
            Matrix B = recvMatrix(new_socket);
            std::cout << "Received B: " << B.rows << "x" << B.cols << std::endl;

            std::cout << "Calculating Product (A * B)..." << std::endl;
            Matrix C = multiplyMatrices(A, B);
            std::cout << "Calculation complete. Result size: " << C.rows << "x" << C.cols << std::endl;
            std::cout << "Sending Result C back to client..." << std::endl;
            sendMatrix(new_socket, C);
            std::cout << "Data sent successfully." << std::endl;

        } catch (const std::exception& e) {
            std::cerr << "Error handling client: " << e.what() << std::endl;
        }

        close(new_socket);
        std::cout << "Client disconnected." << std::endl;
    }

    return 0;
}
