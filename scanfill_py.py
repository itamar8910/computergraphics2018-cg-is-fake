

from PIL import Image
from collections import namedtuple
Point = namedtuple('Point', ['x', 'y'])
WIDTH, HEIGHT = 400, 400
image = Image.new('RGB', (400, 400))
image_pixels = image.load()

def putpixel(x, y, color):
    image_pixels[x, HEIGHT-y -1 ] = color


def draw_line(start, end, color):
    """
    http://www.roguebasin.com/index.php?title=Bresenham%27s_Line_Algorithm#Python
    Bresenham's Line Algorithm
    Produces a list of tuples from start and end
 
    >>> points1 = get_line((0, 0), (3, 4))
    >>> points2 = get_line((3, 4), (0, 0))
    >>> assert(set(points1) == set(points2))
    >>> print points1
    [(0, 0), (1, 1), (1, 2), (2, 3), (3, 4)]
    >>> print points2
    [(3, 4), (2, 3), (1, 2), (1, 1), (0, 0)]
    """
    # Setup initial conditions
    x1, y1 = start
    x2, y2 = end
    dx = x2 - x1
    dy = y2 - y1
 
    # Determine how steep the line is
    is_steep = abs(dy) > abs(dx)
 
    # Rotate line
    if is_steep:
        x1, y1 = y1, x1
        x2, y2 = y2, x2
 
    # Swap start and end points if necessary and store swap state
    swapped = False
    if x1 > x2:
        x1, x2 = x2, x1
        y1, y2 = y2, y1
        swapped = True
 
    # Recalculate differentials
    dx = x2 - x1
    dy = y2 - y1
 
    # Calculate error
    error = int(dx / 2.0)
    ystep = 1 if y1 < y2 else -1
 
    # Iterate over bounding box generating points between start and end
    y = y1
    points = []
    for x in range(x1, x2 + 1):
        coord = (y, x) if is_steep else (x, y)
        points.append(coord)
        error -= abs(dy)
        if error < 0:
            y += ystep
            error += dx
 
    # Reverse the list if the coordinates were swapped
    if swapped:
        points.reverse()
    for point in points:
        putpixel(*point, color)

def draw_trig_outline(trig, color):
    draw_line(trig[0], trig[1], color)
    draw_line(trig[1], trig[2], color)
    draw_line(trig[0], trig[2], color)

def fill_trig(trig, color):
    def get_x_intersection(line, y):
        p1, p2 = line[0], line[1]
        # y = a(x-xstart) + b
        # y = ax - axstart + b
        # x = (y -b)/a + axstart
    
        a = (p2.y - p1.y) / (p2.x - p1.x)
        b = p1.y
        return (y - b)/a + p1.x

    xmin = min([p.x for p in trig])
    xmax = max([p.x for p in trig])
    ymin = min([p.y for p in trig])
    ymax = max([p.y for p in trig])
    lines = [
        sorted((trig[0], trig[1]), key=lambda p: p.x),
        sorted((trig[0], trig[2]), key=lambda p: p.x),
        sorted((trig[1], trig[2]), key=lambda p: p.x),
    ]
    lines = [l for l in lines if l[0].y != l[1].y]  # ignore horizontal lines
    for y in range(ymin, ymax + 1):
        intersections_x = [get_x_intersection(line, y) for line in lines]
        print(intersections_x)
        intersections_x = sorted(set([x for x in intersections_x if xmin <= x <= xmax]))
        print(y, intersections_x)
        if len(intersections_x) != 2:
            continue
        for x in range(int(intersections_x[0]), int(intersections_x[1]) + 1):
            putpixel(x, y, color)

for x in range(WIDTH):
    for y in range(HEIGHT):
        putpixel(x, y, (255, 255, 255))


trig = [Point(50, 300), Point(250, 250), Point(180, 120)]
draw_trig_outline(trig, (255, 0, 0))
fill_trig(trig, (0, 0, 255))
image.show()