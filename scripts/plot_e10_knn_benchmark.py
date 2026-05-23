#!/usr/bin/env python3
"""E10: algorithm wall time on geometric k-NN graphs at three sizes."""
import argparse
import csv
import statistics
from collections import defaultdict

import matplotlib.pyplot as plt
import numpy as np


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--in", dest="inp", default="results/e10_knn_benchmark.csv")
    ap.add_argument("--out", default="results/e10_knn_benchmark.png")
    args = ap.parse_args()

    rows = list(csv.DictReader(open(args.inp)))
    g = defaultdict(list)
    for r in rows:
        g[(r["algo"], int(r["n"]))].append(float(r["wall_ms"]))

    algos = ["kruskal", "prim", "boruvka_seq", "boruvka_omp"]
    ns = sorted({k[1] for k in g})

    fig, ax = plt.subplots(figsize=(9, 5))
    x = np.arange(len(ns))
    width = 0.2
    for i, algo in enumerate(algos):
        ys = [statistics.median(g[(algo, n)]) for n in ns]
        ax.bar(x + (i - 1.5) * width, ys, width, label=algo)
    ax.set_xticks(x)
    ax.set_xticklabels([f"$n={n:,}$" for n in ns])
    ax.set_ylabel("median wall time (ms)")
    ax.set_yscale("log")
    ax.set_title(r"MST wall time on Euclidean $k$-NN graphs, $k=10$")
    ax.legend()
    ax.grid(True, axis="y", which="both", alpha=0.3)
    fig.tight_layout()
    fig.savefig(args.out, dpi=140)
    print(f"wrote {args.out}")


if __name__ == "__main__":
    main()
