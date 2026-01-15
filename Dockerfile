# Stage 1: Build
FROM gcc:15 AS builder
WORKDIR /app
COPY . .
# Compile BOTH source files
RUN g++ -std=c++17 src/main.cpp src/Rate_limiter.cpp -Iinclude -o gatekeeper -pthread -static

# Stage 2: Runtime
FROM alpine:latest
WORKDIR /root/
COPY --from=builder /app/gatekeeper .

# Correct syntax
RUN touch limiter_db.txt

CMD ["./gatekeeper"]