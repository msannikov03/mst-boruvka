#!/usr/bin/env python3
"""E7: speedup heatmap, density vs thread count, for boruvka_omp at n=10^5.

Each row is one edge density; each column is a thread count.
Cell colour shows speedup S_p = T_1(density) / T_p(density), so the diagonal
is bright if scaling is good.
"""
import argparse
import csv
import statistics
from collections import defaultdict

import matplotlib.pyplot as plt
import numpy as np


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--in", dest="inp", default="results/e7_density_heatmap.csv")
    ap.add_argument("--out", default="results/e7_density_heatmap.png")
    args = ap.parse_args()

    rows = list(csv.DictReader(open(args.inp)))
    g = defaultdict(list)
    p_for_density = {}
    for r in rows:
        g[(r["density"], int(r["threads"]))].append(float(r["wall_ms"]))
        p_for_density[r["density"]] = float(r["p"])

    densities = ["sparse", "log", "medium"]
    threads = sorted({k[1] for k in g})

    medians = {k: statistics.median(v) for k, v in g.items()}

    rows_data = []
    for d in densities:
        t1 = medians[(d, threads[0])]
        speedups = [t1 / medians[(d, t)] for t in threads]
        rows_data.append(speedups)
    grid = np.array(rows_data)

    fig, ax = plt.subplots(figsize=(8.5, 4.5))
    im = ax.imshow(grid, aspect="auto", cmap="viridis", origin="lower",
                   vmin=1.0, vmax=max(grid.max(), 1.5))
    ax.set_xticks(range(len(threads)))
    ax.set_xticklabels(threads)
    ax.set_yticks(range(len(densities)))
    ax.set_yticklabels([f"{d}\n$p\\!=\\!{p_for_density[d]:.5g}$" for d in densities])
    ax.set_xlabel("threads $p$")
    ax.set_ylabel("edge density")
    ax.set_title(r"Speedup $S_p = T_1 / T_p$ for boruvka\_omp at $n = 10^5$")
    cbar = fig.colorbar(im, ax=ax)
    cbar.set_label("speedup")

    for i in range(grid.shape[0]):
        for j in range(grid.shape[1]):
            ax.text(j, i, f"{grid[i,j]:.2f}", ha="center", va="center",
                    color="white" if grid[i, j] < grid.mean() else "black", fontsize=9)

    fig.tight_layout()
    fig.savefig(args.out, dpi=140)
    print(f"wrote {args.out}")


if __name__ == "__main__":
    main()
