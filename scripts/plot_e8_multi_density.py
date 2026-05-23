#!/usr/bin/env python3
"""E8: algorithm wall time across three densities at fixed n=10^4."""
import argparse
import csv
import statistics
from collections import defaultdict

import matplotlib.pyplot as plt
import numpy as np


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--in", dest="inp", default="results/e8_multi_density.csv")
    ap.add_argument("--out", default="results/e8_multi_density.png")
    args = ap.parse_args()

    rows = list(csv.DictReader(open(args.inp)))
    g = defaultdict(list)
    for r in rows:
        g[(r["algo"], r["density"])].append(float(r["wall_ms"]))

    algos = ["kruskal", "prim", "boruvka_seq", "boruvka_omp"]
    densities = ["sparse", "log", "dense"]

    fig, ax = plt.subplots(figsize=(9, 5))
    x = np.arange(len(densities))
    width = 0.2
    for i, algo in enumerate(algos):
        ys = [statistics.median(g[(algo, d)]) for d in densities]
        ax.bar(x + (i - 1.5) * width, ys, width, label=algo)
    ax.set_xticks(x)
    ax.set_xticklabels([
        r"sparse $p\!=\!c/n$",
        r"log $p\!=\!c\,\ln(n)/n$",
        r"dense $p\!=\!0.1$",
    ])
    ax.set_ylabel("median wall time (ms)")
    ax.set_yscale("log")
    ax.set_title(r"MST wall time across edge densities, $n = 10^4$")
    ax.legend()
    ax.grid(True, axis="y", which="both", alpha=0.3)
    fig.tight_layout()
    fig.savefig(args.out, dpi=140)
    print(f"wrote {args.out}")


if __name__ == "__main__":
    main()
