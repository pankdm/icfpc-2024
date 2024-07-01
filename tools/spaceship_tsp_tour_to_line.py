import sys
import traceback

# basic converter of LKH output into line

def dist_func(u, v):
    return ((u[0] - v[0])**2 + (u[1] - v[1])**2)**0.5

if __name__ == "__main__":
    if len(sys.argv) != 4:
        raise Exception("bad args")
    with open(sys.argv[1]) as f:
        tsp = f.read()
    tsp = tsp[tsp.find('NODE_COORD_SECTION')+len('NODE_COORD_SECTION'):]
    points = []
    for line in tsp.splitlines():
        line = line.strip()
        if not line:
            continue
        if line == 'EOF':
            break
        points.append(tuple(map(int, line.split())))
    tests_was = set()
    out_idx = 0
    for f_idx in range(1000):
        fname = "%s.v%d"%(sys.argv[2], f_idx)
        try:
            with open(fname) as f:
                tour = f.read()
        except:
            traceback.print_exc()
            break
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
        base = dist_func(points[idx[0]][1:], points[idx[1]][1:])
        other = dist_func(points[idx[0]][1:], points[idx[-1]][1:])
        if other < base:
            idx = idx[0:1] + list(reversed(idx[1:]))
        test = []
        for i in idx[1:]:
            if points[i][0] != i + 1:
                raise Exception(f'wut {points[i][0]} != {i + 1}')
            test.append((points[i][1], points[i][2]))
        test = tuple(test)
        if test not in tests_was:
            tests_was.add(test)
            fname = "%s.v%d"%(sys.argv[3], out_idx)
            out_idx += 1
            print(fname)
            with open(fname, 'w') as f:
                f.write('\n'.join(f'{i[0]} {i[1]}' for i in test))
