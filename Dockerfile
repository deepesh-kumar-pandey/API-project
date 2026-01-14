# STAGE 1: Build
FROM gcc:15 AS builder
WORKDIR /app

# Copy everything (src, include, etc.)
COPY . .

# Compile: -Iinclude tells g++ to look in the include folder for .h files
RUN g++ -std=c++17 src/main.cpp -Iinclude -o gatekeeper -pthread -static

# STAGE 2: Run
FROM alpine:latest
WORKDIR /root/
COPY --from=builder /app/gatekeeper .
RUN touch limiter_db.txt
CMD ["./gatekeeper"]