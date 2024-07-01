import sys

# basic converter into TSPLIB format

if __name__ == "__main__":
    if len(sys.argv) != 2:
        raise Exception("expected EDGE_WEIGHT_TYPE")
    if sys.argv[1] not in ('EUC_2D', 'MAX_2D', 'MAN_2D', 'CEIL_2D'):
        raise Exception(f"invalid EDGE_WEIGHT_TYPE = {sys.argv[1]}")
    points = [(0, 0)]
    was = set()
    was.add((0, 0))
    while True:
        try:
            x, y = map(int, input().split())
        except:
            break
        if (x, y) not in was:
            was.add((x, y))
            points.append((x, y))
    print('NAME : spaceship')
    print('COMMENT : spaceship')
    print('TYPE : TSP')
    print('DIMENSION :', len(points))
    print('EDGE_WEIGHT_TYPE : ', sys.argv[1])
    print('NODE_COORD_SECTION')
    for i in range(len(points)):
        x, y = points[i]
        print(f'{i+1} {x} {y}')
    print('EOF')

