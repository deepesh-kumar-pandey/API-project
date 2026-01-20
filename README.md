# Gatekeeper: High-Performance Rate Limiter 🚀

A production-grade, thread-safe Rate Limiter implemented in **Modern C++**. This service provides a low-latency barrier for APIs, protecting backend resources from traffic spikes, DDoS attempts, and brute-force abuse.

## ✨ Engineering Highlights
* **Low-Latency Design:** Developed in C++ to ensure sub-millisecond overhead per request check, making it suitable for high-frequency trading or high-traffic gateways.
* **Thread Safety:** Utilizes `std::mutex` and RAII-style locking (`std::lock_guard`) to ensure data integrity and prevent race conditions in multi-threaded environments.
* **Algorithm:** Implements **Fixed-Window Counter** logic with a local state persistence layer.
* **Cloud-Native & DevOps Ready:** Fully containerized using multi-stage Docker builds (Alpine Linux) and designed for Infrastructure-as-Code (IaC) deployment.

---

## 🛠️ Tech Stack & Tools
* **Core Logic:** C++11 / C++14 (Standard Library)
* **Concurrency:** Pthreads / C++ Concurrency API
* **Containerization:** Docker (Lightweight Alpine Base)
* **Orchestration & IaC:** Terraform & Kubernetes (Manifests included)
* **Dev Environment:** VS Code, GCC/MinGW, WSL2

---

## 🏗️ System Architecture
The application is designed to function as a microservice security layer that intercepts request metadata before it reaches expensive backend resources.

1.  **Request Ingest:** The service accepts a `user_id` or `api_key`.
2.  **Validation Layer:** The `RateLimiter` class checks the timestamp against the predefined quota.
3.  **Persistence:** State is flushed to `limiter_db.txt` to ensure rate-limit continuity across service restarts or container crashes.

---

## 🚀 Getting Started

### Local Build & Run
```powershell
# Compile the project
g++ -std=c++11 -I include src/main.cpp src/Rate_limiter.cpp -o gatekeeper

# Run the application
./gatekeeper

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

## 🧪 Docker Live Testing

You can run automated tests against the containerized application using these scripts.

### PowerShell Script
```powershell
# Configuration for the test
$TEMP_KEY = "test_secret_123"
$MAX_REQ  = "5"
$WINDOW   = "10"
$USER_ID  = "tester_01"

Write-Host "------------------------------------------------" -ForegroundColor Cyan
Write-Host " Starting Gatekeeper Live Test (PowerShell)     "
Write-Host " Settings: $MAX_REQ requests / $WINDOW seconds  "
Write-Host "------------------------------------------------" -ForegroundColor Cyan

# Grouping commands into a block to pipe into Docker's stdin
& {
    echo $MAX_REQ      # Setting Max Requests
    echo $WINDOW       # Setting Time Window
    
    # Loop to send 10 requests (5 should be ALLOWED, 5 should be DENIED)
    for ($i = 1; $i -le 10; $i++) {
        echo "check $USER_ID"
        Start-Sleep -Milliseconds 100
    }

    echo "status $USER_ID"   # Check final status
    echo "exit"             # Save and close the app
} | docker run -i --rm -e ENCRYPTION_KEY=$TEMP_KEY gatekeeper_app
```

### Git Bash Script
```bash
#!/bin/bash

# Configuration for the test
TEMP_KEY="test_secret_123"
MAX_REQ="5"
WINDOW="10"
USER="tester_01"

echo "------------------------------------------------"
echo " Starting Gatekeeper Live Test (Docker)         "
echo " Settings: $MAX_REQ requests / $WINDOW seconds  "
echo "------------------------------------------------"

# We pipe all commands into the container's stdin
{
  echo "$MAX_REQ"      # Setting Max Requests
  echo "$WINDOW"       # Setting Time Window
  
  # Loop to send 10 requests (5 should pass, 5 should fail)
  for i in {1..10}
  do
    echo "check $USER"
    sleep 0.1          # Small delay to keep logs readable
  done

  echo "status $USER"   # Check final status
  echo "exit"           # Save and close
} | docker run -i --rm -e ENCRYPTION_KEY="$TEMP_KEY" gatekeeper_app
```

### Expected Output
With settings of 5 requests per 10 seconds:
- First 5 `check` commands → `[ALLOWED]`
- Next 5 `check` commands → `[DENIED]`
- Final `status` shows `0 requests left`

---

## 📝 License
This project is open source and available for educational purposes.
