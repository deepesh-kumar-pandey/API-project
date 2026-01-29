import socket
import time
import sys

def send_mock_traffic(port, count=5):
    print(f"Sending {count} mock packets to localhost:{port}...")
    for i in range(count):
        try:
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
                s.settimeout(0.5)
                s.connect(("127.0.0.1", port))
                print(f"Request {i+1} sent.")
        except Exception as e:
            # We expect connection refused if no one is listening on the port,
            # but libpcap will still see the SYN packet!
            print(f"Request {i+1} sent (packet captured despite closed port).")
        time.sleep(0.5)

if __name__ == "__main__":
    port = int(sys.argv[1]) if len(sys.argv) > 1 else 80
    send_mock_traffic(port)
