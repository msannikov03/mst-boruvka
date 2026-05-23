#!/usr/bin/env python3
"""E5: per-round phase decomposition for Borůvka-OMP at n=10^5, 1 vs 10 threads."""
import argparse
import csv
import statistics
from collections import defaultdict

import matplotlib.pyplot as plt
import numpy as np


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--in", dest="inp", default="results/e5_profile.csv")
    ap.add_argument("--out", default="results/e5_profile.png")
    args = ap.parse_args()

    rows = list(csv.DictReader(open(args.inp)))

    g = defaultdict(lambda: {"setup": [], "scan": [], "merge": [], "contract": []})
    for r in rows:
        key = (int(r["threads"]), int(r["round"]))
        g[key]["setup"   ].append(float(r["setup_ms"]))
        g[key]["scan"    ].append(float(r["scan_ms"]))
        g[key]["merge"   ].append(float(r["merge_ms"]))
        g[key]["contract"].append(float(r["contract_ms"]))

    medians = {k: {p: statistics.median(v) for p, v in vs.items()} for k, vs in g.items()}

    thread_counts = sorted({k[0] for k in medians})
    phases = ["setup", "scan", "merge", "contract"]
    colors = ["#7e57c2", "#26a69a", "#ef6c00", "#c62828"]
    labels = {
        "setup":    "setup (uf snapshot + tl reset)",
        "scan":     "scan (parallel min-edge)",
        "merge":    "merge (parallel reduction)",
        "contract": "contract (sequential UF)",
    }

    fig, axes = plt.subplots(1, len(thread_counts), figsize=(11, 4.5), sharey=True)
    if len(thread_counts) == 1:
        axes = [axes]

    for ax, t in zip(axes, thread_counts):
        rounds_t = sorted({k[1] for k in medians if k[0] == t})
        bottoms = np.zeros(len(rounds_t))
        for ph, color in zip(phases, colors):
            heights = np.array([medians[(t, r)][ph] for r in rounds_t])
            ax.bar(rounds_t, heights, bottom=bottoms, color=color, label=labels[ph])
            bottoms += heights
        ax.set_xlabel("Borůvka round")
        ax.set_title(f"{t} thread" + ("s" if t > 1 else ""))
        ax.set_xticks(rounds_t)
        ax.grid(True, axis="y", alpha=0.3)
        if t == thread_counts[0]:
            ax.set_ylabel("median time per round (ms)")

    axes[-1].legend(loc="upper right", fontsize=8)
    fig.suptitle(r"Per-round phase breakdown, $n=10^5$, $p\approx 4\,\ln(n)/n$")
    fig.tight_layout()
    fig.savefig(args.out, dpi=140)
    print(f"wrote {args.out}")


if __name__ == "__main__":
    main()
