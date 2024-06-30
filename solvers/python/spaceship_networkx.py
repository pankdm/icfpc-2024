import networkx as nx
import sys

from threading import Thread, stack_size
stack_size(int(1e8))
sys.setrecursionlimit(1000000)

def dist_func(u, v):
    return (u[0] - v[0])**2 + (u[1] - v[1])**2 


def euler(G, v, res):
    while G[v]:
        x = G[v].pop()
        G[x].pop(G[x].index(v))
        euler(G, x, res)
    res.append(v)

class Euler(Thread):
    def __init__(self, V):
        Thread.__init__(self)
        self.E = E

    def run(self):
        self.result = []
        euler(E, 0, self.result)


def solve(V, E):
    n = len(V)
    vertices = []
    for i in range(n):
        if len(E[i]) & 1:
            vertices.append(i)
    print(len(vertices))
    edges = []
    for i in range(len(vertices)):
        for j in range(i + 1, len(vertices)):
            u = vertices[i]
            v = vertices[j]
            dist = dist_func(V[u], V[v])
            edges.append((i, j, dist))
    G = nx.Graph()
    G.add_weighted_edges_from(edges)
    #print(edges)
    matching = nx.min_weight_matching(G)
    for u, v in matching:
        E[vertices[u]].append(vertices[v])
        E[vertices[v]].append(vertices[u])
    euler_thread = Euler(E)
    euler_thread.start()
    euler_thread.join()
    res = euler_thread.result
    #print(res)
    if res[0] != res[-1] or len(set(res)) != n:
        raise Exception('bad euler path')
    res.pop()
    zero = res.index(0)
    res = res[zero+1:] + res[:zero]
    if len(set(res)) != n - 1 or (0 in res):
        raise Exception('bad rotate')
    if dist_func(V[res[0]], [0,0]) > dist_func(V[res[-1]], [0,0]):
        res.reverse()
    was = set()
    result = []
    for i in res:
        if i not in was:
            was.add(i)
            result.append(V[i])
    return result

if __name__ == '__main__':
    for i in range(1, 25):
        if i == 19: continue
        print(f'running {i}')
        # ret = comm(f"S{interpreter.encode_string(f'get lambdaman{i}')}")
        with open(f"problems/spaceship_mst/spaceship{i}.txt", "rt") as f:
            test = f.read().splitlines()
        test.reverse()
        n = int(test.pop())
        V = []
        for _ in range(n):
            V.append(list(map(int, test.pop().split())))
        E = [[] for i in range(n)]
        for _ in range(n - 1):
            u, v = map(int, test.pop().split())
            E[u].append(v)
            E[v].append(u)
        #print('V')
        #print(*V, sep='\n')
        #print('E')
        #print(*E, sep='\n')
        res = solve(V, E)
        with open(f"problems/spaceship_christofides_serdyukov/spaceship{i}.txt", "wt") as f:
            for i in res:
                print(*i, file=f)
        #print(i, len(maze), len(maze[0]), len(sol), len(sol2))

