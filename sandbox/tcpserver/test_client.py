import socket
import time

def main():
    for _ in range(5):  # Simulate 5 clients
        client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client.connect(('127.0.0.1', 18000))
        time.sleep(1)  # Add a delay between connections

if __name__ == "__main__":
    main()
