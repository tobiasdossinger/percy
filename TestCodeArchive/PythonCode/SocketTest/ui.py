from tkinter import *
import socket
import sys

HOST = '192.168.178.' + str(sys.argv[1])  # The server's hostname or IP address
PORT = 80        # The port used by the server

print('Connecting to ' + HOST + ':' + str(PORT))

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect((HOST, PORT))
print('Connected')

def sendMotorCommand():
    formattedPower = format(int(power.get()), '03d')
    motorID = motor.get()

    dataToSend = str(motorID) + '-' + formattedPower + 'x'

    print(dataToSend)

    sock.sendall(str.encode(dataToSend))

def radioButtonChanged():
    sendMotorCommand()


def sliderChanged(value):
    sendMotorCommand()

master = Tk()
motor = IntVar()
power = IntVar()

Radiobutton(master, text="Motor 1", variable=motor, value=1, command=radioButtonChanged).pack(anchor=W)
Radiobutton(master, text="Motor 2", variable=motor, value=2, command=radioButtonChanged).pack(anchor=W)
Radiobutton(master, text="Motor 3", variable=motor, value=3, command=radioButtonChanged).pack(anchor=W)
Radiobutton(master, text="Motor 4", variable=motor, value=4, command=radioButtonChanged).pack(anchor=W)
w = Scale(master, from_=0, to=255, variable=power, orient=HORIZONTAL, command=sliderChanged)

w.pack()

mainloop()


#!/usr/bin/env python3