#!/usr/bin/env python3
"""E4: Borůvka rounds vs log2(n) against the ⌈log2 n⌉ bound."""
import argparse
import csv
import math
import statistics
from collections import defaultdict

import matplotlib.pyplot as plt
import numpy as np


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--in", dest="inp", default="results/e4_rounds.csv")
    ap.add_argument("--out", default="results/e4_rounds.png")
    args = ap.parse_args()

    rows = list(csv.DictReader(open(args.inp)))
    g = defaultdict(list)
    for r in rows:
        g[int(r["n"])].append(int(r["rounds"]))

    ns = sorted(g)
    fig, ax = plt.subplots(figsize=(8, 5))

    for n in ns:
        rounds = g[n]
        x = [math.log2(n)] * len(rounds)
        ax.scatter(x, rounds, alpha=0.5, color="C0", s=40)

    xs = [math.log2(n) for n in ns]
    medians = [statistics.median(g[n]) for n in ns]
    ax.plot(xs, medians, marker="o", linewidth=2, color="C0",
            label="observed (median across seeds)")

    xs_smooth = np.linspace(min(xs), max(xs), 200)
    ax.plot(xs_smooth, np.ceil(xs_smooth), linestyle=":", color="grey",
            label=r"$\lceil \log_2 n \rceil$ bound")

    ax.set_xlabel(r"$\log_2 n$")
    ax.set_ylabel("Borůvka rounds")
    ax.set_title(r"Borůvka rounds vs graph size, $p \approx 4\,\ln(n)/n$")
    ax.legend()
    ax.grid(True, alpha=0.3)
    fig.tight_layout()
    fig.savefig(args.out, dpi=140)
    print(f"wrote {args.out}")


if __name__ == "__main__":
    main()
