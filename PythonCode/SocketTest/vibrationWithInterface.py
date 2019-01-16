from tkinter import *
import socket

HOST = '192.168.178.44'  # The server's hostname or IP address
PORT = 80        # The port used by the server

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect((HOST, PORT))

def sliderChanged(value):
    sock.sendall(str.encode(value + "\n"))

master = Tk()
w = Scale(master, from_=0, to=200, orient=HORIZONTAL, command=sliderChanged)
w.pack()

mainloop()





#!/usr/bin/env python3



