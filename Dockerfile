# Stage 1: Build the application
FROM gcc:15 AS builder
WORKDIR /app
COPY . .
# Compile BOTH main.cpp and Rate_limiter.cpp
RUN g++ -std=c++17 src/main.cpp src/Rate_limiter.cpp -Iinclude -o gatekeeper -pthread -static

# Stage 2: Runtime environment
FROM alpine:latest
WORKDIR /root/
# Copy the compiled executable from the builder stage
COPY --from=builder /app/gatekeeper .
# Create the database file (The RUN prefix is required)
RUN touch limiter_db.txt
# Set the command to run the app
CMD ["./gatekeeper"]