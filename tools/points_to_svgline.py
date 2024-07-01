import drawsvg as draw
import sys

if __name__ == "__main__":
    if len(sys.argv) != 3:
        raise Exception("3 args expected, got {sys.argv}")
    with open(sys.argv[1], 'r') as f:
        points_text = f.read()
    points = [tuple(map(int, s.split())) for s in points_text.splitlines()]
    min_x = min([p[0] for p in points])
    max_x = max([p[0] for p in points])
    min_y = min([p[1] for p in points])
    max_y = max([p[1] for p in points])
    min_x = min(min_x, 0)
    max_x = max(max_x, 0)
    min_y = min(min_y, 0)
    max_y = max(max_y, 0)
    max_dim = max(max_x-min_x, max_y-min_y)
    if max_dim > 1000000:
        radius = 10000
    elif max_dim > 500000:
        radius = 5000
    elif max_dim > 50000:
        radius = 100
    elif max_dim > 10000:
        radius = 50
    elif max_dim > 1000:
        radius = 10
    elif max_dim > 100:
        radius = 1
    else:
        radius = 0.5
    stroke_width = radius/2
    d = draw.Drawing(max_x - min_x + 10, max_y - min_y + 10, origin=(min_x - 5, min_y -5))
    d.set_render_size(w=1000)
    prev = (0, 0)
    for x, y in points:
        d.append(draw.Line(prev[0], prev[1], x, y,
    stroke='green', stroke_width=stroke_width, fill='none',
    ))
        prev = (x, y)
    for x, y in points:
        d.append(draw.Circle(x, y, radius, fill='red'))
    d.append(draw.Circle(0, 0, radius, fill='blue'))

    d.save_svg(sys.argv[2])

