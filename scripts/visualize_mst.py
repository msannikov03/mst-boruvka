#!/usr/bin/env python3
"""Render an MST: draws every edge of the input graph and highlights the MST.

Reads the output of `tools/dump_mst`:
    line 1: "n m total_weight"
    next m lines: "u v w in_mst"   (in_mst is 0/1)

Layouts:
    spring   force-directed (default)
    grid     interpret vertex i as (i // cols, i % cols); pass --grid-cols
    circular evenly placed on a circle
"""
import argparse
import sys

import matplotlib.pyplot as plt
import networkx as nx


def parse_dump(stream):
    header = stream.readline().split()
    n = int(header[0])
    m = int(header[1])
    weight = float(header[2])
    edges = []
    for _ in range(m):
        parts = stream.readline().split()
        edges.append((int(parts[0]), int(parts[1]), float(parts[2]), int(parts[3])))
    return n, m, weight, edges


def build_layout(n, kind, cols):
    G = nx.Graph()
    G.add_nodes_from(range(n))
    if kind == "spring":
        return nx.spring_layout(G, seed=42)
    if kind == "circular":
        return nx.circular_layout(G)
    if kind == "grid":
        c = cols or int(n ** 0.5)
        return {i: (i % c, -(i // c)) for i in range(n)}
    raise ValueError(kind)


def main():
    p = argparse.ArgumentParser()
    p.add_argument("--in", dest="inp", default="-", help="dump file path or - for stdin")
    p.add_argument("--out", default="mst.png")
    p.add_argument("--layout", choices=["spring", "grid", "circular"], default="spring")
    p.add_argument("--grid-cols", type=int, default=0)
    p.add_argument("--title", default=None)
    args = p.parse_args()

    stream = sys.stdin if args.inp == "-" else open(args.inp)
    n, m, weight, edges = parse_dump(stream)

    G = nx.Graph()
    G.add_nodes_from(range(n))
    for u, v, w, _ in edges:
        G.add_edge(u, v, weight=w)

    pos = build_layout(n, args.layout, args.grid_cols)

    mst_edges = [(u, v) for u, v, _, t in edges if t]
    non_mst = [(u, v) for u, v, _, t in edges if not t]

    fig, ax = plt.subplots(figsize=(8, 8))
    nx.draw_networkx_nodes(G, pos, node_size=180, node_color="#1f2933", ax=ax)
    nx.draw_networkx_edges(G, pos, edgelist=non_mst, edge_color="#cfcfcf",
                           width=0.7, ax=ax)
    nx.draw_networkx_edges(G, pos, edgelist=mst_edges, edge_color="#c0392b",
                           width=2.4, ax=ax)
    title = args.title or f"MST  n={n}  m={m}  weight={weight:.4g}"
    ax.set_title(title)
    ax.set_axis_off()
    fig.tight_layout()
    fig.savefig(args.out, dpi=140)
    print(f"wrote {args.out}", file=sys.stderr)


if __name__ == "__main__":
    main()
