#!/usr/bin/env python3
"""Plot wall-clock time vs thread count from a `bench` CSV.

CSV schema (no header):
    algo,threads,n,m,mst_edges,rounds,total_weight,wall_ms

Single-threaded baselines (kruskal/prim/boruvka_seq) are drawn as horizontal
reference lines; boruvka_omp is plotted across the thread sweep.
"""
import argparse
import csv
import sys
from collections import defaultdict

import matplotlib.pyplot as plt


def main():
    p = argparse.ArgumentParser()
    p.add_argument("--in", dest="inp", default="-")
    p.add_argument("--out", default="bench.png")
    p.add_argument("--title", default=None)
    args = p.parse_args()

    stream = sys.stdin if args.inp == "-" else open(args.inp)
    rows = list(csv.reader(stream))

    series = defaultdict(list)
    for r in rows:
        algo = r[0]
        threads = int(r[1])
        ms = float(r[7])
        series[algo].append((threads, ms))

    fig, ax = plt.subplots(figsize=(8, 5))
    for algo in sorted(series.keys()):
        pts = sorted(series[algo])
        if algo == "boruvka_omp":
            xs = [t for t, _ in pts]
            ys = [ms for _, ms in pts]
            ax.plot(xs, ys, marker="o", linewidth=2.0, label=algo)
        else:
            ms = pts[0][1]
            ax.axhline(ms, linestyle="--", alpha=0.6, label=f"{algo} (1 thr)")

    ax.set_xlabel("threads")
    ax.set_ylabel("wall time (ms)")
    ax.set_yscale("log")
    ax.set_title(args.title or "MST wall time vs threads")
    ax.legend()
    ax.grid(True, alpha=0.3)
    fig.tight_layout()
    fig.savefig(args.out, dpi=140)
    print(f"wrote {args.out}", file=sys.stderr)


if __name__ == "__main__":
    main()
