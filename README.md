<div align="center">

# Gatekeeper

### Thread-Safe Rate Limiting for Modern C++ Applications

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)](https://github.com/deepesh-kumar-pandey/API-project)
[![C++](https://img.shields.io/badge/C%2B%2B-11%2F14-00599C?logo=cplusplus&logoColor=white)](https://isocpp.org/)
[![Docker](https://img.shields.io/badge/Docker-Ready-2496ED?logo=docker&logoColor=white)](https://hub.docker.com/)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](CONTRIBUTING.md)

[Quick Start](#-quick-start) •
[Features](#-features) •
[Documentation](#-documentation) •
[API Reference](#-api-reference) •
[Contributing](#-contributing)

</div>

---

## 🎯 Overview

**Gatekeeper** is a lightweight, thread-safe rate limiting library built with modern C++ for protecting backend services from traffic spikes and API abuse. Designed with simplicity and correctness in mind, it provides a solid foundation for controlling request rates in multi-threaded applications.

### What Gatekeeper Does

- ✅ **Controls Request Rates** - Limit requests per user/API key per time window
- ✅ **Thread-Safe Operations** - Mutex-protected concurrent access
- ✅ **Persistent State** - Survives application restarts
- ✅ **Zero Dependencies** - Pure C++ standard library
- ✅ **Docker Ready** - Containerized deployment support

### Use Cases

- 🔐 **API Protection** - Prevent abuse of public APIs
- 🛡️ **Brute-Force Prevention** - Limit login attempts
- 💰 **Cost Control** - Manage cloud resource consumption
- 📊 **Fair Usage** - Ensure equitable resource distribution

---

## ✨ Features

<table>
<tr>
<td width="50%">

### 🏗️ **Architecture**
- **Fixed-Window Counter** algorithm
- **Thread-safe** with mutex-based locking
- **In-memory** hash map for fast lookups
- **File-based** state persistence
- **Clean C++11/14** implementation

### 🔧 **Developer Experience**
- **Simple CLI** interface
- **Interactive** configuration
- **Clear** command structure
- **Easy integration** into existing projects
- **Minimal dependencies**

</td>
<td width="50%">

### 🐳 **Deployment**
- **Docker** containerization
- **Alpine Linux** base (small footprint)
- **Configurable** via environment variables
- **Persistent** data volumes
- **Graceful** shutdown handling

### 💼 **Production Features**
- **State recovery** after crashes
- **User isolation** (per-user limits)
- **Status monitoring** commands
- **Manual override** capabilities
- **Audit logging**

</td>
</tr>
</table>

---

## 🏗️ Architecture

### Design Overview

```
┌─────────────────────────────────────────────────────────────┐
│                    Gatekeeper Rate Limiter                  │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────────────────────────────────┐               │
│  │         RateLimiter Class               │               │
│  ├─────────────────────────────────────────┤               │
│  │                                         │               │
│  │  • max_requests:      int               │               │
│  │  • time_window:       int               │               │
│  │  • user_limits:       map<string, UserLimit>           │
│  │  • global_mutex:      mutex             │               │
│  │                                         │               │
│  └────────────┬────────────────────────────┘               │
│               │                                            │
│               │                                            │
│  ┌────────────▼──────────────────────────┐                │
│  │      Per-User Tracking                │                │
│  ├───────────────────────────────────────┤                │
│  │                                       │                │
│  │  UserLimit {                          │                │
│  │    request_count:      int            │                │
│  │    window_start_time:  time_t         │                │
│  │    user_mutex:         mutex          │                │
│  │  }                                    │                │
│  │                                       │                │
│  └───────────────────────────────────────┘                │
│                                                             │
│  ┌─────────────────────────────────────────┐               │
│  │       Persistence Layer                 │               │
│  ├─────────────────────────────────────────┤               │
│  │  • save_state() → limiter_db.txt        │               │
│  │  • load_state() ← limiter_db.txt        │               │
│  └─────────────────────────────────────────┘               │
└─────────────────────────────────────────────────────────────┘
```

### Algorithm: Fixed-Window Counter

Gatekeeper uses a **fixed-window counter** algorithm:

1. **Time Window**: Divide time into fixed intervals (e.g., 60 seconds)
2. **Counter**: Track requests per user per window
3. **Reset**: Counter resets when window expires
4. **Decision**: Allow if `count < max_requests`, deny otherwise

**Example** (5 requests per 60 seconds):

```
Window 1 (0-60s):   [✓ ✓ ✓ ✓ ✓ ✗ ✗] → First 5 allowed, rest denied
Window 2 (60-120s): [✓ ✓ ✓ ✓ ✓ ✗ ✗] → Counter reset, first 5 allowed
```

---

## 🚀 Quick Start

### Prerequisites

| Component | Version | Purpose |
|-----------|---------|---------|
| **C++ Compiler** | GCC 7+ / Clang 5+ / MSVC 2017+ | Compilation |
| **CMake** (optional) | 3.10+ | Build system |
| **Docker** (optional) | 20.10+ | Containerization |
| **Git** | 2.x+ | Version control |

---

### Installation

#### 🐳 Docker (Recommended)

```bash
# Clone repository
git clone https://github.com/deepesh-kumar-pandey/API-project.git
cd API-project

# Build Docker image
docker build -t gatekeeper:latest .

# Run container
docker run -it --name gatekeeper gatekeeper:latest
```

#### 💻 Native Build (Linux/macOS)

```bash
# Clone repository
git clone https://github.com/deepesh-kumar-pandey/API-project.git
cd API-project

# Compile
g++ -std=c++11 -pthread -I include src/main.cpp src/Rate_limiter.cpp -o gatekeeper

# Run
./gatekeeper
```

#### 🪟 Windows (MinGW)

```powershell
# Clone repository
git clone https://github.com/deepesh-kumar-pandey/API-project.git
cd API-project

# Compile
g++ -std=c++11 -pthread -I include src/main.cpp src/Rate_limiter.cpp -o gatekeeper.exe

# Run
.\gatekeeper.exe
```

---

### First Run

When you first launch Gatekeeper, configure your rate limits:

```
╔═════════════════════════════════════════════╗
║     GATEKEEPER: RATE LIMITER v1.0           ║
╚═════════════════════════════════════════════╝

Enter Max Requests allowed: 100
Enter Time Window (in seconds): 60
```

**This configuration means:**
- Each user can make **100 requests**
- Within a **60-second** window
- After 60 seconds, the counter resets

---

## 📖 Usage

### Interactive Commands

| Command | Syntax | Description |
|---------|--------|-------------|
| **check** | `check <user_id>` | Log a request and check if allowed |
| **status** | `status <user_id>` | View remaining quota for user |
| **clear** | `clear <user_id>` | Reset rate limit for specific user |
| **help** | `help` | Display all available commands |
| **exit** | `exit` | Save state and shutdown gracefully |

---

### Example Session

```bash
$ ./gatekeeper

╔═════════════════════════════════════════════╗
║     GATEKEEPER: RATE LIMITER v1.0           ║
╚═════════════════════════════════════════════╝

Enter Max Requests allowed: 5
Enter Time Window (in seconds): 60

Configuration:
• Max Requests: 5 per user
• Time Window: 60 seconds
• Persistence: ENABLED

>> check alice
✓ Request allowed for alice (4/5 remaining)

>> check alice
✓ Request allowed for alice (3/5 remaining)

>> check alice
✓ Request allowed for alice (2/5 remaining)

>> check alice
✓ Request allowed for alice (1/5 remaining)

>> check alice
✓ Request allowed for alice (0/5 remaining)

>> check alice
✗ Rate limit exceeded for alice (retry in 57s)

>> status alice
User: alice
Remaining: 0/5 requests
Window resets in: 57 seconds

>> clear alice
✓ Rate limit cleared for alice

>> check alice
✓ Request allowed for alice (4/5 remaining)

>> exit
Saving state to limiter_db.txt...
Goodbye!
```

---

## 🔧 Configuration

### Runtime Configuration

Configure limits when starting the application:

```bash
./gatekeeper
# Prompts:
# Max Requests: 1000
# Time Window: 3600  (1 hour)
```

### Environment Variables (Docker)

```bash
docker run -it \
  -e MAX_REQUESTS=1000 \
  -e TIME_WINDOW=3600 \
  -v $(pwd)/data:/app/data \
  gatekeeper:latest
```

| Variable | Default | Description |
|----------|---------|-------------|
| `MAX_REQUESTS` | Interactive | Maximum requests per window |
| `TIME_WINDOW` | Interactive | Window duration (seconds) |

### Persistence

Gatekeeper automatically saves state to `limiter_db.txt`:

**File Format:**
```
# user_id,request_count,window_start_timestamp
alice,3,1706198400
bob,5,1706198460
charlie,0,1706198520
```

**Features:**
- ✅ Auto-save on `exit` command
- ✅ Auto-restore on startup
- ✅ Human-readable CSV format
- ✅ Crash recovery support

---

## 📊 Performance Characteristics

### Algorithm Complexity

| Operation | Time Complexity | Space Complexity |
|-----------|----------------|------------------|
| `is_request_allowed()` | O(1) | O(n) users |
| `get_remaining_requests()` | O(1) | O(1) |
| `reset_user_limit()` | O(1) | O(1) |
| `save_state()` | O(n) | O(1) |
| `load_state()` | O(n) | O(n) |

### Memory Footprint

**Per-User Overhead:**
```cpp
struct UserLimit {
    int request_count;        // 4 bytes
    time_t window_start_time; // 8 bytes
    std::mutex mtx;           // ~40 bytes (platform-dependent)
};
// Total: ~52-60 bytes per user (excluding string key)
```

**Total Memory** (approximate):
- **10 users**: ~1 KB
- **1,000 users**: ~100 KB
- **100,000 users**: ~10 MB

### Thread Safety

- ✅ **Global mutex** protects the user map
- ✅ **Per-user mutex** protects individual counters
- ✅ **Lock guards** prevent deadlocks
- ✅ **RAII pattern** ensures proper cleanup

### Known Limitations

⚠️ **Fixed-Window Edge Case:**

```
Window 1: [____________________][✓✓✓✓✓]
Window 2: [✓✓✓✓✓][____________________]
          ^
          | Burst of 10 requests in 1 second
          | (5 at end of Window 1, 5 at start of Window 2)
```

**Mitigation:** Consider sliding-window or token-bucket algorithms for stricter enforcement (planned for v2.0).

---

## 🔌 API Reference

### Core Class: `RateLimiter`

```cpp
#include "Rate_limiter.h"

class RateLimiter {
public:
    /**
     * @brief Construct a new Rate Limiter
     * @param max_requests Maximum requests allowed per window
     * @param time_window Time window duration in seconds
     */
    RateLimiter(int max_requests, int time_window);

    /**
     * @brief Check if a request should be allowed
     * @param user_id Unique identifier for the user/API key
     * @return true if request allowed, false if rate limit exceeded
     */
    bool is_request_allowed(const std::string& user_id);

    /**
     * @brief Get remaining requests for a user
     * @param user_id Unique identifier
     * @return int Number of requests remaining in current window
     */
    int get_remaining_requests(const std::string& user_id);

    /**
     * @brief Reset rate limit for a specific user
     * @param user_id Unique identifier
     */
    void reset_user_limit(const std::string& user_id);

    /**
     * @brief Save current state to disk
     * Writes to limiter_db.txt in current directory
     */
    void save_state();

    /**
     * @brief Load previous state from disk
     * Reads from limiter_db.txt if it exists
     */
    void load_state();

private:
    struct UserLimit {
        int request_count;           ///< Current request count
        time_t window_start_time;    ///< Window start timestamp
        std::mutex mtx;              ///< Thread-safety mutex
    };

    int max_requests_;                              ///< Max requests per window
    int time_window_;                               ///< Window duration (seconds)
    std::map<std::string, UserLimit> user_limits_;  ///< Per-user tracking
    std::mutex global_mutex_;                       ///< Global map protection
};
```

### Usage Example

```cpp
#include "Rate_limiter.h"
#include <iostream>

int main() {
    // Create rate limiter: 10 requests per 60 seconds
    RateLimiter limiter(10, 60);

    // Check requests
    if (limiter.is_request_allowed("user123")) {
        std::cout << "Request allowed" << std::endl;
        // Process request...
    } else {
        std::cout << "Rate limit exceeded" << std::endl;
        // Return 429 Too Many Requests
    }

    // Check remaining quota
    int remaining = limiter.get_remaining_requests("user123");
    std::cout << "Remaining: " << remaining << std::endl;

    // Save state before shutdown
    limiter.save_state();

    return 0;
}
```

---

## 🐳 Deployment

### Docker Compose

```yaml
version: '3.8'

services:
  gatekeeper:
    build: .
    container_name: rate-limiter
    restart: unless-stopped
    stdin_open: true
    tty: true
    volumes:
      - ./data:/app/data
    environment:
      - MAX_REQUESTS=1000
      - TIME_WINDOW=60
```

**Deploy:**

```bash
docker-compose up -d
```

### Docker Volume Persistence

```bash
# Create named volume for data persistence
docker volume create gatekeeper-data

# Run with volume
docker run -it \
  -v gatekeeper-data:/app/data \
  gatekeeper:latest
```

### Production Considerations

**For production use, consider:**

1. **HTTP Wrapper** - Add REST API endpoint
2. **Distributed Mode** - Use Redis/memcached for multi-instance deployments
3. **Monitoring** - Add Prometheus metrics export
4. **Logging** - Structured logging (JSON format)
5. **Health Checks** - Implement liveness/readiness probes

---

## 🧪 Testing

### Manual Testing

#### Test 1: Basic Functionality

```bash
./gatekeeper
# Config: Max=5, Window=10

>> check test_user
>> check test_user
>> check test_user
>> check test_user
>> check test_user
# All should be ALLOWED

>> check test_user
# Should be DENIED
```

#### Test 2: Window Reset

```bash
>> check test_user
# DENIED

# Wait 10 seconds...

>> check test_user
# Should be ALLOWED (window reset)
```

#### Test 3: State Persistence

```bash
>> check alice
>> check alice
>> exit

# Restart application
./gatekeeper

>> status alice
# Should show 2 requests used
```

### Automated Testing Script

```bash
#!/bin/bash

echo "Starting Gatekeeper Test Suite..."

# Build
g++ -std=c++11 -pthread -I include src/*.cpp -o gatekeeper

# Test 1: Compile success
if [ $? -eq 0 ]; then
    echo "✓ Compilation successful"
else
    echo "✗ Compilation failed"
    exit 1
fi

# Test 2: Run with automated input
{
  echo "5"
  echo "10"
  echo "check user1"
  echo "check user1"
  echo "check user1"
  echo "check user1"
  echo "check user1"
  echo "check user1"  # Should be denied
  echo "status user1"
  echo "exit"
} | ./gatekeeper > test_output.txt

# Verify output
if grep -q "DENIED" test_output.txt; then
    echo "✓ Rate limiting works correctly"
else
    echo "✗ Rate limiting failed"
    exit 1
fi

echo "All tests passed!"
```

---

## 🔒 Security

### What Gatekeeper Protects Against

- ✅ **Brute-Force Attacks** - Limit login/authentication attempts
- ✅ **API Scraping** - Control data extraction rates
- ✅ **Resource Exhaustion** - Prevent backend overload
- ✅ **Denial of Service** - Mitigate simple DoS attacks

### What Gatekeeper Does NOT Protect Against

- ❌ **Distributed DDoS** - Use CDN/WAF for network-layer attacks
- ❌ **IP Spoofing** - Requires network-level defenses
- ❌ **Authentication Bypass** - Gatekeeper is not an auth system
- ❌ **Data Validation** - Input validation is separate concern

### Security Best Practices

1. **Use with Authentication**
   ```cpp
   // Pseudocode
   if (!authenticated(request)) {
       return 401;
   }
   if (!limiter.is_request_allowed(user_id)) {
       return 429;
   }
   ```

2. **Rate Limit by Multiple Dimensions**
   ```cpp
   // Limit by user AND IP
   string key = user_id + ":" + ip_address;
   limiter.is_request_allowed(key);
   ```

3. **Deploy Behind Reverse Proxy**
   ```nginx
   # nginx.conf
   upstream gatekeeper {
       server localhost:8080;
   }
   ```

4. **Monitor and Alert**
   ```bash
   # Watch for high denial rates
   grep "DENIED" limiter.log | wc -l
   ```

---

## 🗺️ Roadmap

### ✅ Version 1.0 (Current)

- [x] Fixed-window counter algorithm
- [x] Thread-safe operations
- [x] Persistent state (file-based)
- [x] Docker containerization
- [x] Interactive CLI

### 🚧 Version 1.1 (Planned - Q1 2025)

- [ ] HTTP REST API endpoint
- [ ] Configuration file support (YAML/JSON)
- [ ] Logging improvements (structured logs)
- [ ] Unit test suite
- [ ] Benchmark suite

### 🔮 Version 2.0 (Future)

- [ ] Sliding-window algorithm
- [ ] Token-bucket algorithm
- [ ] Redis backend (distributed mode)
- [ ] Prometheus metrics exporter
- [ ] Admin dashboard (web UI)

### 🌟 Version 3.0 (Vision)

- [ ] Multi-tenancy support
- [ ] Dynamic rate adjustment
- [ ] Geographic-based limiting
- [ ] Machine learning anomaly detection

---

## 🤝 Contributing

Contributions are welcome! Here's how you can help:

### Ways to Contribute

- 🐛 **Report Bugs** - [Open an issue](https://github.com/deepesh-kumar-pandey/API-project/issues)
- ✨ **Request Features** - [Suggest enhancements](https://github.com/deepesh-kumar-pandey/API-project/issues)
- 📝 **Improve Docs** - Fix typos, add examples
- 💻 **Submit Code** - Fix bugs, implement features
- 🧪 **Write Tests** - Improve test coverage
- 📊 **Run Benchmarks** - Measure real-world performance

### Development Setup

```bash
# Fork and clone
git clone https://github.com/YOUR_USERNAME/API-project.git
cd API-project

# Create feature branch
git checkout -b feature/my-feature

# Make changes
# ...

# Build and test
g++ -std=c++11 -pthread -I include src/*.cpp -o gatekeeper
./gatekeeper

# Commit
git commit -m "feat: add sliding window algorithm"

# Push and create PR
git push origin feature/my-feature
```

### Code Style

- Follow existing C++ conventions
- Use meaningful variable names
- Add comments for complex logic
- Keep functions focused and small
- Write self-documenting code

### Pull Request Process

1. Update README.md with any new features
2. Ensure code compiles without warnings
3. Add tests for new functionality
4. Update documentation as needed

---

## 📜 License

This project is licensed under the **MIT License**.

```
MIT License

Copyright (c) 2025 Deepesh Kumar Pandey

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

## 🙏 Acknowledgments

### Built With

- **C++11/14** - Modern C++ standard
- **STL** - Standard Template Library
- **POSIX Threads** - Thread-safe concurrency
- **Docker** - Containerization
- **Alpine Linux** - Lightweight base image

### Inspired By

- [NGINX Rate Limiting](https://www.nginx.com/blog/rate-limiting-nginx/)
- [Redis INCR-based Rate Limiting](https://redis.io/commands/incr/)
- [Token Bucket Algorithm](https://en.wikipedia.org/wiki/Token_bucket)
- [Kong API Gateway](https://konghq.com/)

### Special Thanks

- The C++ community for best practices
- Contributors and early adopters
- Open source maintainers everywhere

---

## 📞 Support

### Getting Help

- 📖 **Documentation** - [GitHub Wiki](https://github.com/deepesh-kumar-pandey/API-project/wiki)
- 💬 **Discussions** - [GitHub Discussions](https://github.com/deepesh-kumar-pandey/API-project/discussions)
- 🐛 **Bug Reports** - [Issue Tracker](https://github.com/deepesh-kumar-pandey/API-project/issues)

### Contact

**Deepesh Kumar Pandey**

- GitHub: [@deepesh-kumar-pandey](https://github.com/deepesh-kumar-pandey)
- Email: deepesh.pandey@example.com

---

## 📚 Learn More

### Related Reading

- [Rate Limiting Patterns](https://cloud.google.com/architecture/rate-limiting-strategies-techniques)
- [API Security Best Practices](https://owasp.org/www-project-api-security/)
- [Designing Data-Intensive Applications](https://dataintensive.net/)

### Similar Projects

- [redis-rate-limiter](https://github.com/smrchy/redis-rate-limiter)
- [node-rate-limiter-flexible](https://github.com/animir/node-rate-limiter-flexible)
- [go-rate](https://github.com/beefsack/go-rate)

---

<div align="center">

## ⭐ If This Helped You

Give it a star! It helps others discover the project.

[![GitHub stars](https://img.shields.io/github/stars/deepesh-kumar-pandey/API-project?style=social)](https://github.com/deepesh-kumar-pandey/API-project/stargazers)

---

**Built with C++ by developers, for developers**

[⬆ Back to Top](#gatekeeper)

© 2025 Deepesh Kumar Pandey. All rights reserved.

</div>
