#!/usr/bin/env python

CELL_INSULATOR = 0
CELL_CONDUCTOR = 1
CELL_SPARKTAIL = 2
CELL_SPARKHEAD = 3 

def perimeter(x1, y1, x2, y2):
    '''Generator for each x,y position in a rectangular perimeter'''
    for n in range(x1,  x2+1):  yield n,    y1
    for n in range(y1+1,y2+1):  yield x2,   n
    for n in range(1, x2-x1):   yield x2-n, y2
    for n in range(0, y2-y1):   yield x1,   y2-n

def in_bounds(w, h):
    '''Parameterizes and returns a function to create generators'''
    def closure(pos):
        x, y = pos
        return (x>=0)and(x<w)and(y>=0)and(y<h)
    return closure

def plane(w,h):
    return ((x,y) for x in range(w) for y in range(h))

def matrix(w,h):
    row = [0]*w
    m = [row]
    for y in range(h-1): m.append(list(row))
    return m

# Currently this is an incredibly naive WireWorld implementation, mostly
# here as a placeholder for future functionality to get the GUI working
class WireWorld:
    def __init__(self, w, h):
        self.w = w
        self.h = h
        self.front_buffer = matrix(w,h)
        self.back_buffer = matrix(w,h)

    def get_at(x,y): return self.front_buffer[x][y]

    def step(self):
        w, h = self.w, self.h
        bounds = in_bounds(w, h)
        for x, y in plane(w, h):
            v = self.front_buffer[x][y]

            if   v == CELL_INSULATOR: pass
            elif v == CELL_CONDUCTOR:
                v = sum((self.get_at(x,y) for x,y in
                      filter(bounds, perimeter(x-1,y-1,x+1,y+1))))
                if 3 > v and v > 0: v = CELL_ELECTRONHEAD
                else: v = CELL_CONDUCTOR
            else: v -= 1

            self.back_buffer[x][y] = v

