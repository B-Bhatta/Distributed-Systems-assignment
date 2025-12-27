# Distributed-Systems-assignment

# Distributed Systems: Server-Client Matrix Multiplication

**Student:** Bhaskarjyoti Bhattacharyya  
**Roll No:** 220710007017  
**Language:** C++ (TCP Sockets)

## Project Overview
This project implements a **Distributed Computing System** capable of offloading heavy computational tasks (Matrix Multiplication) from a Client node to a centralized Server node using **C++ Sockets**.

Unlike Python implementations that rely on `pickle`, this project manually handles the TCP/IP stack, including custom binary serialization, buffer management, and reliable packet transmission.

## Features
- **Custom Protocol:** Implements a binary header-payload protocol for efficient data transfer.
- **Robust Networking:** Handles TCP fragmentation (partial packet delivery) ensuring data integrity for large matrices (N=500+).
- **High Performance:** Uses C++ memory management and O(N^3) algorithmic logic.

## Project Structure
| File | Description |
|------|-------------|
| `server.cpp` | The compute node. Listens on port 8080, accepts connections, and performs multiplication. |
| `client.cpp` | The request node. Generates random matrices, sends them to the server, and measures RTT (Round Trip Time). |
| `matrix_utils.h` | Shared library handling socket streams, serialization, and matrix data structures. |

## How to Compile & Run

### 1. Compilation
Use `g++` to compile both the server and client.
```bash
g++ server.cpp -o server
g++ client.cpp -o client
