import socket

s = socket.socket()
host = '192.168.178.55' # needs to be in quote
port = 8080
s.connect((host, port))
print('Connected')
print('Data', s.recv(1024))
inpt = input('type anything and click enter... ')
s.send(bytes(inpt, 'utf-8'))
print("the message has been sent")