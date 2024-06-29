import drawsvg as draw
import os

if __name__ == "__main__":
    os.makedirs("svgs", exist_ok=True)
    for f in os.listdir("problems/spaceship"):
        if not f.startswith("raw_"):
            with open(f"problems/spaceship/{f}", "r") as source:
                points = [s.split() for s in source.readlines()]
                points = [(int(p[0]), int(p[1])) for p in points]
                min_x = min([p[0] for p in points])
                max_x = max([p[0] for p in points])
                min_y = min([p[1] for p in points])
                max_y = max([p[1] for p in points])
                d = draw.Drawing(max_x - min_x + 10, max_y - min_y + 10, origin='center')
                d.set_render_size(w=1000)
                for x, y in points:
                    d.append(draw.Circle(x, y, 1, fill='red'))

                d.save_svg(f"svgs/{f}.svg")
