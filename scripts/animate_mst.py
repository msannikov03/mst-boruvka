#!/usr/bin/env python3
"""Animate Borůvka MST construction round by round.

Replays the algorithm in Python so per-round state is captured without touching
the C++ tools. Reads an edge-list file (`n m` header, then `m` lines of `u v w`)
from stdin or --in, writes a GIF (one frame per round) to --out.
"""
import argparse
import math
import sys

import matplotlib.pyplot as plt
import networkx as nx
from matplotlib.animation import FuncAnimation, PillowWriter


def parse_graph(stream):
    header = stream.readline().split()
    n = int(header[0])
    m = int(header[1])
    edges = []
    for _ in range(m):
        u, v, w = stream.readline().split()
        edges.append((int(u), int(v), float(w)))
    return n, edges


class UF:
    def __init__(self, n):
        self.p = list(range(n))

    def find(self, x):
        while self.p[x] != x:
            self.p[x] = self.p[self.p[x]]
            x = self.p[x]
        return x

    def unite(self, a, b):
        ra, rb = self.find(a), self.find(b)
        if ra == rb:
            return False
        lo, hi = (ra, rb) if ra < rb else (rb, ra)
        self.p[hi] = lo
        return True


def boruvka_rounds(n, edges):
    """Yield list of edge indices added in each round."""
    uf = UF(n)
    in_mst = [False] * len(edges)
    while True:
        cheapest = {}
        for k, (u, v, w) in enumerate(edges):
            cu, cv = uf.find(u), uf.find(v)
            if cu == cv:
                continue
            for root in (cu, cv):
                cur = cheapest.get(root)
                if cur is None or (w, k) < (cur[0], cur[1]):
                    cheapest[root] = (w, k, u, v)
        added = []
        for w, k, u, v in cheapest.values():
            if in_mst[k] or uf.find(u) == uf.find(v):
                continue
            if uf.unite(u, v):
                in_mst[k] = True
                added.append(k)
        if not added:
            return
        yield added


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--in", dest="inp", default="-")
    ap.add_argument("--out", default="results/mst_animation.gif")
    ap.add_argument("--layout", choices=["grid", "spring", "circular"], default="grid")
    ap.add_argument("--grid-cols", type=int, default=0)
    ap.add_argument("--fps", type=float, default=1.0)
    args = ap.parse_args()

    stream = sys.stdin if args.inp == "-" else open(args.inp)
    n, edges = parse_graph(stream)

    G = nx.Graph()
    G.add_nodes_from(range(n))
    for u, v, w in edges:
        G.add_edge(u, v, weight=w)

    if args.layout == "grid":
        c = args.grid_cols or int(round(math.sqrt(n)))
        pos = {i: (i % c, -(i // c)) for i in range(n)}
    elif args.layout == "spring":
        pos = nx.spring_layout(G, seed=42)
    else:
        pos = nx.circular_layout(G)

    mst_set = set()
    frames = [frozenset(mst_set)]
    for added in boruvka_rounds(n, edges):
        mst_set |= set(added)
        frames.append(frozenset(mst_set))

    fig, ax = plt.subplots(figsize=(8, 8))

    def draw(i):
        ax.clear()
        ax.set_axis_off()
        in_mst = frames[i]
        mst_pairs = [(edges[k][0], edges[k][1]) for k in in_mst]
        non_mst  = [(edges[k][0], edges[k][1]) for k in range(len(edges)) if k not in in_mst]
        nx.draw_networkx_edges(G, pos, edgelist=non_mst, edge_color="#cfcfcf",
                               width=0.6, ax=ax)
        nx.draw_networkx_edges(G, pos, edgelist=mst_pairs, edge_color="#c0392b",
                               width=2.4, ax=ax)
        nx.draw_networkx_nodes(G, pos, node_size=140, node_color="#1f2933", ax=ax)
        if i == 0:
            title = "before round 1"
        elif i == len(frames) - 1:
            title = f"final MST  ({len(in_mst)} edges)"
        else:
            title = f"after Borůvka round {i}  ({len(in_mst)} MST edges)"
        ax.set_title(title)
        return []

    anim = FuncAnimation(fig, draw, frames=len(frames), interval=1000 / args.fps,
                         blit=False, repeat=True)
    anim.save(args.out, writer=PillowWriter(fps=args.fps))
    print(f"wrote {args.out} ({len(frames)} frames)", file=sys.stderr)


if __name__ == "__main__":
    main()
