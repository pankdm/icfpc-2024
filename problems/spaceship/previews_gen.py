import os, sys

def generate_svg(id):
    dirname = f'{os.path.dirname(__file__)}'
    with open(f'{dirname}/spaceship{id}.txt', 'r') as file:
        data = file.read().strip()
    points = [s.split(' ') for s in data.split('\n')]
    points = [[int(x), int(y)] for x,y  in points]
    xmin = min(p[0] for p in points)
    xmax = max(p[0] for p in points)
    ymin = min(p[1] for p in points)
    ymax = max(p[1] for p in points)
    w = xmax - xmin
    h = ymax - ymin
    rmin = min(w,h)
    rmax = max(w,h)
    desired_size = 800
    point_r = 0.5
    scale = desired_size / (max(w,h))
    with open(f'{dirname}/previews/spaceship{id}.svg', 'w') as file:
        file.write(f'<svg xmlns="http://www.w3.org/2000/svg" width="{desired_size}" height="{desired_size}">\n')
        file.write(f'<g transform="translate({(desired_size-(w)*scale)/2} {(desired_size-(h)*scale)/2}) scale({scale})">\n')
        file.write(f'<rect fill="#000000" width="{xmax-xmin}" height="{ymax-ymin}"></rect>\n')
        file.write(f'<g fill="#ffffff" transform="translate(0, {ymax-ymin}) scale(1, -1)">\n')
        for [x,y] in points:
            file.write(f'<ellipse cx="{x-xmin}" cy="{y-ymin}" rx="{point_r}" ry="{point_r}"></ellipse>\n')
        file.write(f'</g>\n')
        file.write(f'</g>\n')
        file.write('</svg>')

if __name__ == '__main__':
    for id in range(1,26):
        generate_svg(id)
