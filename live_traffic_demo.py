import socket
import time
import random
import sys

def simulate_massive_traffic(port, num_users=50, delay=0.1):
    print(f"🚀 Starting Live High-User Demo on port {port}")
    print(f"👥 Simulating {num_users} unique users with a {delay}s delay...")
    
    users = [f"192.168.{random.randint(1, 254)}.{random.randint(1, 254)}" for _ in range(num_users)]
    
    start_time = time.time()
    packet_count = 0
    
    try:
        while True:
            user_ip = random.choice(users)
            # print(f"🔗 Packet detected from: {user_ip}") # Quieter for performance
            
            try:
                with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
                    s.settimeout(0.01)
                    s.connect_ex(("127.0.0.1", port))
                    packet_count += 1
            except:
                pass
            
            time.sleep(delay)
    except KeyboardInterrupt:
        end_time = time.time()
        duration = end_time - start_time
        pps = packet_count / duration if duration > 0 else 0
        print("\n" + "="*40)
        print("🛑 Demo Performance Summary")
        print("="*40)
        print(f"Total Packets Sent: {packet_count}")
        print(f"Total Duration:     {duration:.2f} s")
        print(f"Throughput:        {pps:.2f} packets/sec")
        print("="*40)


if __name__ == "__main__":
    port_arg = int(sys.argv[1]) if len(sys.argv) > 1 else 80
    simulate_massive_traffic(port_arg)
