from tkinter import *
import socket
import sys

HOST = '192.168.178.' + str(sys.argv[1])  # The server's hostname or IP address
PORT = 8080        # The port used by the server

print('Connecting to ' + HOST + ':' + str(PORT))

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect((HOST, PORT))
print('Connected')

def sendMotorCommand(motor):
    formattedPower = format(int(power.get()), '03d')

    dataToSend = str(motor) + '-' + str(pattern.get()) + '-' + formattedPower + 'x'

    print(dataToSend)

    sock.sendall(str.encode(dataToSend))

def activateMotor(currentMotor):
    sendMotorCommand(currentMotor)

window = Tk()
window.title('Percy')
patternFrame = Frame(window)

power = IntVar()
pattern = IntVar()

Radiobutton(patternFrame, text="Muster 1", variable=pattern, value=0).pack()
Radiobutton(patternFrame, text="Muster 2", variable=pattern, value=1).pack()
Radiobutton(patternFrame, text="Muster 3", variable=pattern, value=2).pack()
patternFrame.grid(row=0, column=0, padx='5', pady='5', sticky='ew')


motorButton1 = Button(window, text='Motor 1', command= lambda: activateMotor(0))
motorButton2 = Button(window, text='Motor 2', command= lambda: activateMotor(1))
motorButton3 = Button(window, text='Motor 3', command= lambda: activateMotor(2))
motorButton4 = Button(window, text='Motor 4', command= lambda: activateMotor(3))
motorButton1.grid(row=2, column=0, padx='5', pady='5', sticky='ew')
motorButton2.grid(row=2, column=1, padx='5', pady='5', sticky='ew')
motorButton3.grid(row=2, column=2, padx='5', pady='5', sticky='ew')
motorButton4.grid(row=2, column=3, padx='5', pady='5', sticky='ew')


durationSlider = Scale(window, from_=0, to=5000, resolution=50, variable=power, orient=HORIZONTAL)
durationSlider.grid(row=1, column=0, columnspan=4, padx='5', pady='5', sticky='ew')
# w.pack()

mainloop()


#!/usr/bin/env python3