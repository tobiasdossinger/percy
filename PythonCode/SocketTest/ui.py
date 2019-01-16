from tkinter import *

def sliderChanged(value):
    print(value)

master = Tk()
w = Scale(master, from_=0, to=200, orient=HORIZONTAL, command=sliderChanged)
w.pack()

mainloop()