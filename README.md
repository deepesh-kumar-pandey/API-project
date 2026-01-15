# Rate Limiter API 🚀

A high-performance, thread-safe Rate Limiter implemented in **Modern C++**. This service protects APIs from abuse by limiting requests a user can make within a specific time window.

## ✨ Features
* **C++11 Compatible:** Clean, portable code that works with C++11 and above.
* **Thread-Safe:** Handles concurrent requests safely using mutex locks.
* **Persistent Storage:** Saves and loads rate-limit data from `limiter_db.txt`.
* **Dockerized:** Ready for deployment using a lightweight Docker build.

---

## 🛠️ Tech Stack
* **Language:** C++11+
* **Compiler:** GCC / MinGW-w64 / MSYS2
* **Containerization:** Docker (Alpine Linux base)
* **Development Environment:** VS Code on Windows

---

## 🚀 Getting Started

### Prerequisites
* GCC compiler with C++11 support
* (Optional) [Docker Desktop](https://www.docker.com/products/docker-desktop/) for containerized deployment
* (Optional) [MSYS2](https://www.msys2.org/) for Windows builds

### Building Locally

1. **Compile the project:**
   ```powershell
   g++ -std=c++11 -I include src/main.cpp src/Rate_limiter.cpp -o gatekeeper.exe
   ```

2. **Run the application:**
   ```powershell
   .\gatekeeper.exe
   ```

### Running with Docker

1. **Build the image:**
   ```powershell
   docker build -t rate-limiter-api .
   ```

2. **Run the container:**
   ```powershell
   docker run -it --name my-api rate-limiter-api
   ```

---

## 📖 Usage

When the application starts, configure your rate limiter:
```
Enter Max Requests allowed: 5
Enter Time Window (in seconds): 60
```

### Commands
| Command | Description |
|---------|-------------|
| `check <user_id>` | Log a request for a user |
| `status <user_id>` | View remaining requests |
| `clear <user_id>` | Reset user's rate limit |
| `help` | Show available commands |
| `exit` | Save data and quit |

### Example Session
```
>> check alice
[ALLOWED] Request logged for alice | Remaining: 4/5
>> check alice
[ALLOWED] Request logged for alice | Remaining: 3/5
>> status alice
User alice has 3 requests left.
>> exit
Data saved. Goodbye!
```

---

## 📂 Project Structure
```text
Rate_limiter-API/
├── src/
│   ├── main.cpp           # CLI interface
│   └── Rate_limiter.cpp   # Rate limiter implementation
├── include/
│   └── Rate_limiter.h     # Header file
├── Dockerfile             # Docker configuration
└── README.md              # Project documentation
```

---

## 📝 License
This project is open source and available for educational purposes.