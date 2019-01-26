import tkinter as tk

import config as cfg
from basicAStar import astar

def drawRectangle(x, y, color = "#CCC"):
    x = x * cfg.GRID_SIZE
    y = y * cfg.GRID_SIZE
    c.create_rectangle(x, y, x + cfg.GRID_SIZE, y + cfg.GRID_SIZE, fill=color)

def createGrid():
    w = c.winfo_width() # Get current width of canvas
    h = c.winfo_height() # Get current height of canvas
    c.delete('grid_line') # Will only remove the grid_line

    path = astar(cfg.MAZE, cfg.START, cfg.END)

    for step, coords in enumerate(path):
        drawRectangle(coords[0], coords[1], '#999')

    for y, row in enumerate(cfg.MAZE):
        for x, isBlocked in enumerate(row):
            if isBlocked:
                drawRectangle(x, y, '#333')

    drawRectangle(cfg.START[0], cfg.START[1], '#F00')
    drawRectangle(cfg.END[0], cfg.END[1], '#0F0')

    # Creates all vertical lines at intevals of 100
    # for i in range(0, w, 50):
    #     c.create_line([(i, 0), (i, h)], tag='grid_line')

    # # Creates all horizontal lines at intevals of 100
    # for i in range(0, h, 50):
    #     c.create_line([(0, i), (w, i)], tag='grid_line')

def initialize(event=None):
    cfg.GRID_WIDTH = len(cfg.MAZE[0])
    cfg.GRID_HEIGHT = len(cfg.MAZE)
    cfg.GRID_SIZE = min(c.winfo_width() / cfg.GRID_WIDTH, c.winfo_height() / cfg.GRID_HEIGHT)
    print('WINDOW', c.winfo_width(), c.winfo_height())
    print('GRID', cfg.GRID_WIDTH, cfg.GRID_HEIGHT, cfg.GRID_SIZE)
    createGrid()


root = tk.Tk()
c = tk.Canvas(root, height=1000, width=1000, bg='white')
c.pack(fill=tk.BOTH, expand=True)
c.bind('<Configure>', initialize)

root.mainloop()