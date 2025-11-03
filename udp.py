import socket

# Create UDP socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Bind to host & port
host = '0.0.0.0'  # Accept messages from any client
port = 6000
server_socket.bind((host, port))
print(f"UDP Server listening on {host}:{port}...")

while True:
    # Receive message from client
    data, addr = server_socket.recvfrom(1024)
    message = data.decode()
    if message.lower() == 'bye':
        print(f"Client {addr} disconnected.")
        break
    print(f"Client ({addr}): {message}")
    
    # Reply to client
    reply = input("Server: ")
    server_socket.sendto(reply.encode(), addr)
    if reply.lower() == 'bye':
        print("Server closed the connection.")
        break




import socket

# Create UDP socket
client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Server IP and port (replace with actual server IP)
server_ip = '192.168.1.5'
port = 6000

print("UDP Client ready. Type 'bye' to exit.")

while True:
    message = input("Client: ")
    client_socket.sendto(message.encode(), (server_ip, port))
    
    if message.lower() == 'bye':
        print("Client closed the connection.")
        break
    
    data, addr = client_socket.recvfrom(1024)
    reply = data.decode()
    if reply.lower() == 'bye':
        print("Server closed the connection.")
        break
    print(f"Server: {reply}")