import socket

s = socket.socket()
host = '192.168.178.55' # needs to be in quote
port = 8080
s.connect((host, port))
# print('Connected')
# while True:
#     print('Data', s.recv(1024))






def recvall(conn):
    data = ""
    while True:
        try:
            data = conn.recv(1024)
            return data
        except ValueError:
            continue

            
while True:
    data = recvall(s)
    print(data)