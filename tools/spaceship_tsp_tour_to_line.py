import sys

# basic converter of LKH output into line

def dist_func(u, v):
    return ((u[0] - v[0])**2 + (u[1] - v[1])**2)**0.5

if __name__ == "__main__":
    if len(sys.argv) != 3:
        raise Exception("bad args")
    with open(sys.argv[1]) as f:
        tsp = f.read()
    with open(sys.argv[2]) as f:
        tour = f.read()
    tsp = tsp[tsp.find('NODE_COORD_SECTION')+len('NODE_COORD_SECTION'):]
    points = []
    for line in tsp.splitlines():
        line = line.strip()
        if not line:
            continue
        if line == 'EOF':
            break
        points.append(tuple(map(int, line.split())))
    tour = tour[tour.find('TOUR_SECTION')+len('TOUR_SECTION'):]
    idx = []
    for line in tour.splitlines():
        line = line.strip()
        if not line:
            continue
        if line == 'EOF' or line == '-1':
            break
        idx.append(int(line) - 1)
    if idx[0] != 0:
        raise Exception('expect first point = 0')
    if len(idx) != len(points):
        raise Exception('len(idx) != len(points)')
    base = dist_func(points[0][1:], points[1][1:])
    other = dist_func(points[0][1:], points[-1][1:])
    if other < base:
        idx = idx[0] + list(reversed(idx[1:]))
    for i in idx[1:]:
        if points[i][0] != i + 1:
            raise Exception(f'wut {points[i][0]} != {i + 1}')
        print(points[i][1], points[i][2])

