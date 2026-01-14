# Rate Limiter API 🚀

A high-performance, thread-safe Rate Limiter implemented in **Modern C++ (C++17)**. This service is designed to protect APIs from abuse by limiting the number of requests a user can make within a specific time window.

## ✨ Features
* **C++17 Standard:** Utilizes modern features like Structured Bindings for clean, readable code.
* **Thread-Safe:** Designed to handle concurrent requests safely.
* **Persistent Storage:** Automatically saves and loads rate-limit data from a local database file (`limiter_db.txt`).
* **Dockerized:** Ready for deployment using a lightweight, multi-stage Docker build.

---

## 🛠️ Tech Stack
* **Language:** C++17
* **Compiler:** GCC 15+ (MinGW-w64 / MSYS2)
* **Containerization:** Docker (Alpine Linux base)
* **Development Environment:** VS Code on Windows

---

## 🚀 Getting Started

### Prerequisites
* [Docker Desktop](https://www.docker.com/products/docker-desktop/) installed and running.
* (Optional) [MSYS2](https://www.msys2.org/) if you wish to build locally without Docker.

### Running with Docker (Recommended)
The easiest way to run the application is using the provided Dockerfile. This ensures a consistent environment regardless of your OS.

1.  **Build the image:**
    ```powershell
    docker build -t rate-limiter-api .
    ```

2.  **Run the container:**
    ```powershell
    docker run -it --name my-api rate-limiter-api
    ```

---

## 📂 Project Structure
```text
Rate_limiter-API/
├── src/                # Source files (.cpp)
├── include/            # Header files (.h)
├── Dockerfile          # Multi-stage Docker configuration
├── .gitignore          # Files to exclude from Git
└── README.md           # Project documentation
