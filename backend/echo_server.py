import socket
import sys

def run_backend(port):
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind(('127.0.0.1', port))
    server_socket.listen(1)
    print(f"Backend server listening on port {port}...")
    
    while True:
        client_socket, _ = server_socket.accept()
        print(f"Received connection on port {port}")
        message = client_socket.recv(1024).decode()
        print(f"Received: {message}")
        client_socket.send(f"Echo from backend {port}: {message}".encode())
        client_socket.close()

if __name__ == "__main__":
    port = int(sys.argv[1])
    run_backend(port)
