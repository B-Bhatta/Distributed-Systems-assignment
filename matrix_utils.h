// matrix_utils.h
#ifndef MATRIX_UTILS_H
#define MATRIX_UTILS_H

#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <stdexcept>
#include <random>

struct Matrix {
    int rows;
    int cols;
    std::vector<int> data;

    Matrix(int r = 0, int c = 0) : rows(r), cols(c) {
        data.resize(r * c, 0);
    }

    int& operator()(int r, int c) {
        return data[r * cols + c];
    }

    const int& operator()(int r, int c) const {
        return data[r * cols + c];
    }
};

bool send_all(int socket, const void* buffer, size_t length) {
    const char* ptr = static_cast<const char*>(buffer);
    while (length > 0) {
        ssize_t i = send(socket, ptr, length, 0);
        if (i < 1) return false;
        ptr += i;
        length -= i;
    }
    return true;
}

bool recv_all(int socket, void* buffer, size_t length) {
    char* ptr = static_cast<char*>(buffer);
    while (length > 0) {
        ssize_t i = recv(socket, ptr, length, 0);
        if (i < 1) return false;
        ptr += i;
        length -= i;
    }
    return true;
}


void sendMatrix(int socket, const Matrix& mat) {
    // 1. Send Dimensions (Rows and Cols)
    int dimensions[2] = {mat.rows, mat.cols};
    if (!send_all(socket, dimensions, sizeof(dimensions))) {
        throw std::runtime_error("Failed to send matrix dimensions");
    }

    // 2. Send the Raw Data
    size_t dataSize = mat.data.size() * sizeof(int);
    if (!send_all(socket, mat.data.data(), dataSize)) {
        throw std::runtime_error("Failed to send matrix data");
    }
}

Matrix recvMatrix(int socket) {

    int dimensions[2];
    if (!recv_all(socket, dimensions, sizeof(dimensions))) {
        throw std::runtime_error("Failed to receive matrix dimensions");
    }
    Matrix mat(dimensions[0], dimensions[1]);
    size_t dataSize = mat.data.size() * sizeof(int);
    if (!recv_all(socket, mat.data.data(), dataSize)) {
        throw std::runtime_error("Failed to receive matrix data");
    }

    return mat;
}
void randomizeMatrix(Matrix& mat) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, 9);

    for (int& val : mat.data) {
        val = distrib(gen);
    }
}
void printMatrix(const Matrix& mat) {
    if (mat.rows > 10) {
        std::cout << "[Matrix too large to print...]" << std::endl;
        return;
    }
    for (int i = 0; i < mat.rows; ++i) {
        for (int j = 0; j < mat.cols; ++j) {
            std::cout << mat(i, j) << " ";
        }
        std::cout << std::endl;
    }
}

#endif
