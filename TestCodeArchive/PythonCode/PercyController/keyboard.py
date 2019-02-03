from pynput import keyboard
from time import sleep
import socket
import sys

HOST = '192.168.178.' + str(sys.argv[1])
PORT = 8080

print('Connecting to ' + HOST + ':' + str(PORT))

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect((HOST, PORT))
print('Connected')

def sendMotorCommand(motor, pattern, power):
    dataToSend = str(motor) + '-' + str(pattern) + '-' + str(power) + 'x'

    print(dataToSend)

    sock.sendall(str.encode(dataToSend))

# The currently active modifiers
currentlyPressed = set()

def move(direction, status):
    sendMotorCommand(direction, -1, 2000)
    print(direction, status)

def on_press(key):
    currentKey = str(key)
    if currentKey not in currentlyPressed:
        if currentKey == '\'w\'':
            move(0, 1)
        elif currentKey == '\'a\'':
            move(1, 1)
        elif currentKey == '\'s\'':
            move(2, 1)
        elif currentKey == '\'d\'':
            move(3, 1)
    currentlyPressed.add(currentKey)

def on_release(key):
    try:
        currentKey = str(key)
        if currentKey in currentlyPressed:
            if currentKey == '\'w\'':
                move(0, 0)
            elif currentKey == '\'a\'':
                move(1, 0)
            elif currentKey == '\'s\'':
                move(2, 0)
            elif currentKey == '\'d\'':
                move(3, 0)
        currentlyPressed.remove(currentKey)
    except KeyError:
        pass

listener = keyboard.Listener(on_press=on_press, on_release=on_release)
listener.start()
listener.join()

while True:
    print(currentlyPressed)
    sleep(0.1)