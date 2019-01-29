
import heapq

###########
# HELPERS #
###########



###########
# HELPERS #
###########

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
    frontier = PriorityQueue()
    frontier.put(start, 0)
    came_from = {}
    cost_so_far = {}
    came_from[start] = None
    cost_so_far[start] = 0
    previous = start

    while not frontier.empty():
        current = frontier.get()
        
        if current == goal:
            break

        print()
        print()
        print(current)
        
        for next in graph.neighbors(current):
            new_cost = cost_so_far[current] + graph.cost(current, next)
            print()
            print('    ', next)
            if(previous[0] != next[0] and previous[1] != next[1]):
                print('    ', 'turn')
                new_cost += 6
            print(
                '    ',
                '(', previous[0], ',', previous[1], ')', '->',
                '(', current[0], ',', current[1], ')', '->',
                '(', next[0], ',', next[1], ')',
                '(', new_cost, ')'
            )
            if next not in cost_so_far or new_cost < cost_so_far[next]:
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

        previous = current
    
    return came_from, cost_so_far




def reconstruct_path(came_from, start, goal):
    current = goal
    path = []
    while current != start:
        path.append(current)
        current = came_from[current]
    path.append(start) # optional
    path.reverse() # optional
    return path

def draw_tile(graph, id, style, width):
    r = "."
    if 'number' in style and id in style['number']: r = "%d" % style['number'][id]
    if 'point_to' in style and style['point_to'].get(id, None) is not None:
        (x1, y1) = id
        (x2, y2) = style['point_to'][id]
        if x2 == x1 + 1: r = ">"
        if x2 == x1 - 1: r = "<"
        if y2 == y1 + 1: r = "v"
        if y2 == y1 - 1: r = "^"
    if 'path' in style and id in style['path']: r = "@"
    if 'weights' in style and id in graph.weights: r = graph.weights[id]
    if id in graph.walls: r = "#" #* width
    if 'start' in style and id == style['start']: r = 'A'
    if 'goal' in style and id == style['goal']: r = 'Z'
    return r

def draw_grid(graph, width=2, **style):
    for y in range(graph.height):
        for x in range(graph.width):
            print("%%-%ds" % width % draw_tile(graph, (x, y), style, width), end="")
        print()

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

# print(diagram4.weights)
# print()
# start, goal = (1, 4), (7, 8)
# came_from, cost_so_far = a_star_search(diagram4, start, goal)
# print()
# print()
# print()
# print()
# print(came_from)
# print()
# print()
# print(cost_so_far)
# print()
# print()
# print()
# print()
# draw_grid(diagram4, width=3, weights=diagram4, start=start, goal=goal)
# print()
# draw_grid(diagram4, width=3, point_to=came_from, start=start, goal=goal)
# print()
# draw_grid(diagram4, width=3, number=cost_so_far, start=start, goal=goal)
# print()
# draw_grid(diagram4, width=3, path=reconstruct_path(came_from, start=start, goal=goal))
# print()
# print()
# print()
# print()
# print(reconstruct_path(came_from, start=start, goal=goal))









diagram4 = GridWithWeights(20, 20)
diagram4.walls = [(2, 9), (2, 8), (3, 9), (3, 8), (4, 3), (4, 4), (4, 5), (4, 6), (4, 7), (4, 8), (4, 9)]

innerRing = build_weights_around_walls(diagram4, diagram4.walls, 9)
centerRing = build_weights_around_walls(diagram4, innerRing, 6)
outerRing = build_weights_around_walls(diagram4, centerRing, 3)

start, goal = (1, 6), (7, 8)
came_from, cost_so_far = a_star_search(diagram4, start, goal)


draw_grid(diagram4, width=3, weights=diagram4, start=start, goal=goal)
print()
draw_grid(diagram4, width=3, point_to=came_from, start=start, goal=goal)
print()
draw_grid(diagram4, width=3, number=cost_so_far, start=start, goal=goal)
print()
draw_grid(diagram4, width=3, path=reconstruct_path(came_from, start=start, goal=goal))