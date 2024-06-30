import drawsvg as draw
import os

if __name__ == "__main__":
    os.makedirs("svgs", exist_ok=True)
    for f in os.listdir("problems/spaceship"):
        if not f.startswith("raw_") and f != 'previews' and not f.endswith('.py'):
            with open(f"problems/spaceship/{f}", "r") as source:
                points = [s.split() for s in source.readlines()]
                points = [(int(p[0]), int(p[1])) for p in points]
                min_x = min([p[0] for p in points])
                max_x = max([p[0] for p in points])
                min_y = min([p[1] for p in points])
                max_y = max([p[1] for p in points])
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
                d = draw.Drawing(max_x - min_x + 10, max_y - min_y + 10, origin=(min_x - 5, min_y -5))
                d.set_render_size(w=1000)
                d.append(draw.Circle(0, 0, radius, fill='blue'))
                for x, y in points:
                    d.append(draw.Circle(x, y, radius, fill='red'))

                d.save_svg(f"svgs/{f}.svg")
