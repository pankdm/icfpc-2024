from language.interpreter import ICFPInterpreter, run_fast
from language.gen import *

def solve_maze(maze):
    n, m = len(maze), len(maze[0])
    for i in range(n):
        for j in range(m):
            if maze[i][j] == 'L':
                x0, y0 = i, j
    ans = ''
    last_visited_at = 0
    visited = set()
    rev = {'R': 'L', 'L': 'R', 'U': 'D', 'D': 'U'}
    def dfs(x, y):
        nonlocal ans, last_visited_at
        for dx, dy, c in [(0, 1, 'R'), (0, -1, 'L'), (1, 0, 'D'), (-1, 0, 'U')]:
            x1, y1 = x + dx, y + dy
            if 0 <= x1 < n and 0 <= y1 < m and maze[x1][y1] != '#' and (x1, y1) not in visited:
                visited.add((x1, y1))
                ans += c
                last_visited_at = len(ans)
                dfs(x1, y1)
                ans += rev[c]
    dfs(x0, y0)
    return ans[:last_visited_at]

def rle(s):
    ans = ''
    i = 0
    while i < len(s):
        j = i
        while j < len(s) and s[j] == s[i]:
            j += 1
        ans += s[i] + str(j - i)
        i = j
    return ans

def encode_sol(sol, task_i):
    code = 1
    for c in sol:
        code = code * 4 + "RULD".index(c)
    
    @lam
    def f(rec, n):
        return If(n == 1, "", cat(rec(n / 4), str2expr("RULD").tail(n % 4).head_(1)))
    
    expr = Yc(f, code)
    # assert sol == run_fast(str(expr))
    return str(cat(f"solve lambdaman{task_i} ", expr))

def encode_sol2(sol, task_i):
    return interpreter.encode_string(f"solve lambdaman{task_i} {sol}")

for i in range(1, 22):
    # ret = comm(f"S{interpreter.encode_string(f'get lambdaman{i}')}")
    with open(f"problems/lambdaman/raw_lambdaman{i}.txt", "rt") as f:
        maze = run_fast(f.read())
    l1 = len(rle(maze))
    maze = maze.split()
    sol = encode_sol(solve_maze(maze), i)
    sol2 = encode_sol2(solve_maze(maze), i)
    with open(f"solutions/lambdaman/trivial/{i}.txt", "wt") as f:
        f.write(sol)
    print(i, len(maze), len(maze[0]), len(sol), len(sol2))