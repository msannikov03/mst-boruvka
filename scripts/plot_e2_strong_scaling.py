#!/usr/bin/env python3
"""E2: speedup S_p and parallel efficiency E_p curves for Borůvka-OMP."""
import argparse
import csv
import statistics
from collections import defaultdict

import matplotlib.pyplot as plt


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--in", dest="inp", default="results/e2_strong_scaling.csv")
    ap.add_argument("--out", default="results/e2_strong_scaling.png")
    args = ap.parse_args()

    rows = list(csv.DictReader(open(args.inp)))
    g = defaultdict(list)
    for r in rows:
        g[int(r["threads"])].append(float(r["wall_ms"]))

    ts = sorted(g)
    medians = {t: statistics.median(g[t]) for t in ts}
    t1 = medians[1]
    speedup = [t1 / medians[t] for t in ts]
    efficiency = [s / t for s, t in zip(speedup, ts)]

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(11, 4.5))
    ax1.plot(ts, speedup, marker="o", linewidth=2, color="C0")
    ax1.plot(ts, ts, linestyle=":", color="grey", label="ideal $S_p = p$")
    ax1.set_xlabel("threads $p$")
    ax1.set_ylabel("speedup $S_p = T_1 / T_p$")
    ax1.set_title("Strong scaling, $n=10^5$")
    ax1.legend()
    ax1.grid(True, alpha=0.3)

    ax2.plot(ts, efficiency, marker="s", linewidth=2, color="C1")
    ax2.axhline(1.0, linestyle=":", color="grey", label="ideal $E_p = 1$")
    ax2.set_xlabel("threads $p$")
    ax2.set_ylabel(r"parallel efficiency $E_p = S_p / p$")
    ax2.set_ylim(0, 1.1)
    ax2.set_title("Parallel efficiency")
    ax2.legend()
    ax2.grid(True, alpha=0.3)

    fig.tight_layout()
    fig.savefig(args.out, dpi=140)
    print(f"wrote {args.out}")


if __name__ == "__main__":
    main()
