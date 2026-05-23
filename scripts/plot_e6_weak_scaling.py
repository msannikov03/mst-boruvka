#!/usr/bin/env python3
"""E6: weak scaling — wall time and parallel efficiency vs thread count.

Ideal weak scaling keeps wall time constant; any growth shows imperfect scaling.
"""
import argparse
import csv
import statistics
from collections import defaultdict

import matplotlib.pyplot as plt


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--in", dest="inp", default="results/e6_weak_scaling.csv")
    ap.add_argument("--out", default="results/e6_weak_scaling.png")
    args = ap.parse_args()

    rows = list(csv.DictReader(open(args.inp)))
    g = defaultdict(list)
    for r in rows:
        g[int(r["threads"])].append(float(r["wall_ms"]))

    ts = sorted(g)
    medians = {t: statistics.median(g[t]) for t in ts}
    t1 = medians[ts[0]]
    efficiency = [t1 / medians[t] for t in ts]

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(11, 4.5))

    ax1.plot(ts, [medians[t] for t in ts], marker="o", linewidth=2, color="C0")
    ax1.axhline(t1, linestyle=":", color="grey", label=f"ideal (constant {t1:.1f} ms)")
    ax1.set_xlabel("threads $p$  (and $n = 10^4 \\times p$)")
    ax1.set_ylabel("median wall time (ms)")
    ax1.set_title("Weak scaling (work per thread held constant)")
    ax1.legend()
    ax1.grid(True, alpha=0.3)

    ax2.plot(ts, efficiency, marker="s", linewidth=2, color="C1")
    ax2.axhline(1.0, linestyle=":", color="grey", label="ideal $T_1/T_p = 1$")
    ax2.set_xlabel("threads $p$")
    ax2.set_ylabel("weak-scaling efficiency $T_1 / T_p$")
    ax2.set_ylim(0, 1.1)
    ax2.set_title("Weak-scaling efficiency")
    ax2.legend()
    ax2.grid(True, alpha=0.3)

    fig.tight_layout()
    fig.savefig(args.out, dpi=140)
    print(f"wrote {args.out}")


if __name__ == "__main__":
    main()
