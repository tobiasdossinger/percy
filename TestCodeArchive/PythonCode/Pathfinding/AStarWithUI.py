import tkinter as tk
import heapq

import config as cfg
# from basicAStar import astar


''' HELPER FUNCTIONS '''

def reconstruct_path(came_from, start, goal):
    current = goal
    path = []
    while current != start and current in came_from:
        path.append(current)
        current = came_from[current]
    # path.append(start) # optional
    # path.reverse() # optional
    return path

def drawRectangle(x, y, config):
    x = x * cfg.GRID_SIZE
    y = y * cfg.GRID_SIZE
    c.create_rectangle(x, y, x + cfg.GRID_SIZE, y + cfg.GRID_SIZE, fill=config['color'])
    if 'text' in config:
        c.create_text(
            x + cfg.GRID_SIZE / 2, 
            y + cfg.GRID_SIZE / 2, 
            width=cfg.GRID_SIZE, 
            text=config['text'], 
            justify="center",
            font=('Arial', '10')
        )

def draw_tile(graph, id, style):
    options = {"text": str(id[0]) + ', ' + str(id[1]), "color": '#990'}
    if 'number' in style and id in style['number']: 
        options['text'] += '\n' + "%d" % style['number'][id]
    else:
        options['text'] = '-'
    if 'point_to' in style and style['point_to'].get(id, None) is not None:
        (x1, y1) = id
        (x2, y2) = style['point_to'][id]
        if x2 == x1 + 1: options['text'] += "\n>"
        if x2 == x1 - 1: options['text'] += "\n<"
        if y2 == y1 + 1: options['text'] += "\nv"
        if y2 == y1 - 1: options['text'] += "\n^"
    #"text": graph.weights[id]
    if id in graph.weights: 
        options['color'] = '#' + str(3* format(int(graph.weights[id] / 9 * 255), 'x'))
        options['text'] += '\n' + str(graph.weights[id])
    else:
        options['text'] += '\n1'
    if id in graph.walls: options = {"Text": "#", "color": "#000"}
    if 'path' in style and id in style['path']: options['color'] = '#F28D3C'
    if 'start' in style and id == style['start']: options = {"text": 'A', "color": '#0F0'}
    if 'goal' in style and id == style['goal']: options = {"text": 'Z', "color": '#F00'}
    if id == style['current']: options['color'] = '#0FF'
    if id == style['previous']: options['color'] = '#099'
    drawRectangle(id[0], id[1], options)
    return options

def draw_grid(graph, **style):
    for y in range(graph.height):
        for x in range(graph.width):
            draw_tile(graph, (x, y), style)

def build_weights_around_walls(grid, blocks, weight):
    outerBlocks = []

    for block in blocks:
        (blockX, blockY) = block
        directions = [(blockX+1, blockY), (blockX, blockY-1), (blockX-1, blockY), (blockX, blockY+1)]
        for i in directions:
            if i in grid.weights:
                grid.weights[i] = max(grid.weights[i], weight)
            else:
                grid.weights[i] = weight
        outerBlocks += directions
    
    return outerBlocks


''' PATHFINDING '''

class SquareGrid:
    def __init__(self, width, height):
        self.width = width
        self.height = height
        self.walls = []
    
    def in_bounds(self, id):
        (x, y) = id
        return 0 <= x < self.width and 0 <= y < self.height
    
    def passable(self, id):
        return id not in self.walls
    
    def neighbors(self, id):
        (x, y) = id
        results = [(x+1, y), (x, y-1), (x-1, y), (x, y+1)]
        # results = [(x+1, y), (x, y-1), (x-1, y), (x, y+1), (x+1, y+1), (x+1, y-1), (x-1, y+1), (x-1, y-1)]
        if (x + y) % 2 == 0: results.reverse() # aesthetics
        results = filter(self.in_bounds, results)
        results = filter(self.passable, results)
        return results

class GridWithWeights(SquareGrid):
    def __init__(self, width, height):
        super().__init__(width, height)
        self.weights = {}
    
    def cost(self, from_node, to_node):
        return self.weights.get(to_node, 1)

class PriorityQueue:
    def __init__(self):
        self.elements = []
    
    def empty(self):
        return len(self.elements) == 0
    
    def put(self, item, priority):
        heapq.heappush(self.elements, (priority, item))
    
    def get(self):
        return heapq.heappop(self.elements)[1]

def heuristic(a, b):
    (x1, y1) = a
    (x2, y2) = b
    return abs(x1 - x2) + abs(y1 - y2) * 0.001

def a_star_search(graph, start, goal):
    global limit

    i = 0
    frontier = PriorityQueue()
    frontier.put(start, 0)
    came_from = {}
    cost_so_far = {}
    came_from[start] = None
    cost_so_far[start] = 0
    previous = start

    while not frontier.empty():
        i += 1
        current = frontier.get()

        if current == goal:
            return came_from, cost_so_far, current, previous

        # print()
        # print()
        # print(current)
        
        for next in graph.neighbors(current):
            new_cost = cost_so_far[current] + graph.cost(current, next)
            previous = came_from[current]

            # print()
            # print('    ', next)
            if(previous):
                if(previous[0] != next[0] and previous[1] != next[1]):
                    # print('    ', 'turn')
                    new_cost += 5
                # print(
                #     '    ',
                #     '(', previous[0], ',', previous[1], ')', '->',
                #     '(', current[0], ',', current[1], ')', '->',
                #     '(', next[0], ',', next[1], ')',
                #     '(', new_cost, ')'
                # )
            if next not in cost_so_far or new_cost < cost_so_far[next]:
                # print('    ', 'Best so far')
                cost_so_far[next] = new_cost
                priority = new_cost + heuristic(goal, next)
                # dx1 = current[0] - goal[0]
                # dy1 = current[1] - goal[1]
                # dx2 = start[0] - goal[0]
                # dy2 = start[1] - goal[1]
                # cross = abs(dx1*dy2 - dx2*dy1)
                # priority += cross*0.001
                frontier.put(next, priority)
                came_from[next] = current
                # print('current', current, 'new_cost', new_cost, 'cost_so_far', cost_so_far, 'priority', priority, 'came_from', came_from)
                # print()

        if i > limit:
            return came_from, cost_so_far, current, previous

    return came_from, cost_so_far, current, previous

''' INTERFACE '''

# def createGrid():
#     w = c.winfo_width() # Get current width of canvas
#     h = c.winfo_height() # Get current height of canvas
#     c.delete('grid_line') # Will only remove the grid_line

#     path = astar(cfg.MAZE, cfg.START, cfg.END)

#     for step, coords in enumerate(path):
#         drawRectangle(coords[0], coords[1], '#999')

#     for y, row in enumerate(cfg.MAZE):
#         for x, isBlocked in enumerate(row):
#             if isBlocked:
#                 drawRectangle(x, y, '#333')

#     drawRectangle(cfg.START[0], cfg.START[1], '#F00')
#     drawRectangle(cfg.END[0], cfg.END[1], '#0F0')

    # Creates all vertical lines at intevals of 100
    # for i in range(0, w, 50):
    #     c.create_line([(i, 0), (i, h)], tag='grid_line')

    # # Creates all horizontal lines at intevals of 100
    # for i in range(0, h, 50):
    #     c.create_line([(0, i), (w, i)], tag='grid_line')

def initialize(event=None):
    global limit

    c.delete("all")

    cfg.GRID_SIZE = min(c.winfo_width() / cfg.GRID_WIDTH, c.winfo_height() / cfg.GRID_HEIGHT)
    # print('WINDOW', c.winfo_width(), c.winfo_height())
    # print('GRID', cfg.GRID_WIDTH, cfg.GRID_HEIGHT, cfg.GRID_SIZE)
    
    diagram4 = GridWithWeights(cfg.GRID_WIDTH, cfg.GRID_HEIGHT)
    diagram4.walls = [(0, 7), (1, 7), (2, 7), (2, 9), (2, 8), (3, 9), (3, 8), (4, 3), (4, 4), (4, 5), (4, 6), (4, 7), (4, 8), (4, 9)]

    innerRing = build_weights_around_walls(diagram4, diagram4.walls, 9)
    centerRing = build_weights_around_walls(diagram4, innerRing, 6)
    outerRing = build_weights_around_walls(diagram4, centerRing, 3)

    start, goal = (1, 6), (6, 11)
    came_from, cost_so_far, current, previous = a_star_search(diagram4, start, goal)
    # print()
    # print()
    # # draw_grid(diagram4, width=3, walls=diagram4, start=start, goal=goal)
    # print()
    # # draw_grid(diagram4, width=3, weights=diagram4, start=start, goal=goal)
    # print()
    draw_grid(
        diagram4, 
        width=3, 
        current=current, 
        previous=previous, 
        path=reconstruct_path(came_from, start=start, goal=goal), 
        number=cost_so_far, 
        point_to=came_from, 
        start=start, 
        goal=goal
    )
    # createGrid()

def slider_changed(val):
    global limit
    
    limit = int(val)
    initialize()

limit = 0

root = tk.Tk()
c = tk.Canvas(root, height=900, width=1000, bg='white')
c.grid(row=0, column=0)#.pack(fill=tk.BOTH, expand=True)
c.bind('<Configure>', initialize)

tk.Scale(root, from_=0, to=175, length=500, command=slider_changed).grid(row=0, column=1)

root.mainloop()