# Hyperliquid C++ Client

A high-performance C++ implementation for interacting with the Hyperliquid exchange. 

## 🚀 Current Features
* **Zero-Allocation Orderbook:** Uses fixed-size arrays for ultra-low latency updates.
* **Fast Parsing:** Implements `std::from_chars` for high-speed string-to-float conversion.
* **WebSocket Integration:** Live L2 book updates via Boost.Beast.

## 🛠 Project Structure
* `include/`: Header files (API definitions)
* `src/`: Implementation files

## 📦 Requirements
* C++17 Compiler
* Boost (Beast, Asio, System)
* OpenSSL
* nlohmann/json

## 🔨 How to Build
1. Clone the repository.
2. Run CMake:
   ```bash
   cmake -B build
   cmake --build build
