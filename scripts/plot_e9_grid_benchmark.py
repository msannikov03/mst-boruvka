#!/usr/bin/env python3
"""E9: algorithm wall time on 2D grids of three sizes (32^2, 128^2, 512^2)."""
import argparse
import csv
import statistics
from collections import defaultdict

import matplotlib.pyplot as plt
import numpy as np


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--in", dest="inp", default="results/e9_grid_benchmark.csv")
    ap.add_argument("--out", default="results/e9_grid_benchmark.png")
    args = ap.parse_args()

    rows = list(csv.DictReader(open(args.inp)))
    g = defaultdict(list)
    for r in rows:
        size = f"{r['rows']}x{r['cols']}"
        g[(r["algo"], size)].append(float(r["wall_ms"]))

    algos = ["kruskal", "prim", "boruvka_seq", "boruvka_omp"]
    sizes = ["32x32", "128x128", "512x512"]

    fig, ax = plt.subplots(figsize=(9, 5))
    x = np.arange(len(sizes))
    width = 0.2
    for i, algo in enumerate(algos):
        ys = [statistics.median(g[(algo, s)]) for s in sizes]
        ax.bar(x + (i - 1.5) * width, ys, width, label=algo)
    ax.set_xticks(x)
    ax.set_xticklabels([
        r"$32^2$ ($n\!=\!1024$)",
        r"$128^2$ ($n\!=\!16384$)",
        r"$512^2$ ($n\!=\!262144$)",
    ])
    ax.set_ylabel("median wall time (ms)")
    ax.set_yscale("log")
    ax.set_title("MST wall time on 2D random-weight grids")
    ax.legend()
    ax.grid(True, axis="y", which="both", alpha=0.3)
    fig.tight_layout()
    fig.savefig(args.out, dpi=140)
    print(f"wrote {args.out}")


if __name__ == "__main__":
    main()
