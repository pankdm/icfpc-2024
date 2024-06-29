import sys


class Prog:
    def __init__(self):
        self.time = 0
        self.x = 0
        self.y = 0

        # main state
        self.state = {}
    
    def to_str_padded(self):
        xmin = min(x for (x,y) in self.state.keys())
        ymin = min(y for (x,y) in self.state.keys())

        xmax = max(x for (x,y) in self.state.keys())
        ymax = max(y for (x,y) in self.state.keys())
        
        # print(f"[{xmin}, {ymin}] to [{xmax}, {ymax}]")

        cells = []
        width = 0
        for y in range(ymin, ymax + 1):
            row = []
            for x in range(xmin, xmax + 1):
                key = (x, y)
                val = self.state.get(key, ".")
                str_val = str(val)
                width = max(width, len(str_val))
                row.append(str_val)
            cells.append(row)
        
        # print (f"width = {width}")
        padded = []
        for row in cells:
            padded_row = []
            for val in row:
                padded_row.append(" " * (width - len(val)) + val)
            padded.append(padded_row)

        res = []
        for row in padded:
            res.append(" ".join(row))
        return "\n".join(res)


    def print(self):
        res = []
        print(f"[t={self.time}, x={self.x}, y={self.y}]")
        padded = self.to_str_padded()
        print (res)


    # non-mutating step
    def step(self):
        # our answer
        p = Prog()
        p.time = self.time + 1                
        


def parse_prog(s):
    p = Prog()
    for y, row in enumerate(s.split("\n")):
        for x, val in enumerate(row.split(" ")):
            if val != ".":
                p.state[(x, y)] = val
    return p

# returns a map for the initial state
def read_prog(file):
    with open(file, "r") as f:
        s = f.read()
        return parse_prog(s)


if __name__ == "__main__":
    file = sys.argv[1]
    prog = read_prog(file)
    prog.print()
