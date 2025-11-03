import socket

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
host = '0.0.0.0'
port = 5000
server_socket.bind((host, port))
server_socket.listen(1)
print(f"Server is listening on {host}:{port}...")

conn, addr = server_socket.accept()
print(f"Connected by {addr}")

while True:
    data = conn.recv(1024).decode()
    if not data or data.lower() == 'bye':
        print("Client disconnected.")
        break
    print("Client:", data)
    
    reply = input("Server: ")
    conn.send(reply.encode())
    if reply.lower() == 'bye':
        print("Server closed the connection.")
        break

conn.close()
server_socket.close()



import socket

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_ip = '192.168.1.5'  # Replace with server IP
port = 5000

client_socket.connect((server_ip, port))
print("Connected to server. Type 'bye' to exit.")

while True:
    message = input("Client: ")
    client_socket.send(message.encode())
    if message.lower() == 'bye':
        print("Client closed the connection.")
        break
    
    data = client_socket.recv(1024).decode()
    if data.lower() == 'bye':
        print("Server closed the connection.")
        break
    print("Server:", data)

client_socket.close()