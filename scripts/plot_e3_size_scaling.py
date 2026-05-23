#!/usr/bin/env python3
"""E3: Borůvka-OMP wall time vs graph size, 1 thread vs 10 threads, log-log."""
import argparse
import csv
import statistics
from collections import defaultdict

import matplotlib.pyplot as plt


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--in", dest="inp", default="results/e3_size_scaling.csv")
    ap.add_argument("--out", default="results/e3_size_scaling.png")
    args = ap.parse_args()

    rows = list(csv.DictReader(open(args.inp)))
    g = defaultdict(list)
    for r in rows:
        g[(int(r["n"]), int(r["threads"]))].append(float(r["wall_ms"]))

    ts = sorted({k[1] for k in g})
    ns = sorted({k[0] for k in g})

    fig, ax = plt.subplots(figsize=(8, 5))
    for t in ts:
        ys = [statistics.median(g[(n, t)]) for n in ns]
        ax.plot(ns, ys, marker="o", linewidth=2,
                label=f"{t} thread" + ("s" if t > 1 else ""))

    t1 = ts[0]
    base = statistics.median(g[(ns[0], t1)])
    ax.plot(ns, [base * n / ns[0] for n in ns],
            linestyle=":", color="grey", alpha=0.6, label="linear in $n$")

    ax.set_xlabel("vertices $n$")
    ax.set_ylabel("median wall time (ms)")
    ax.set_xscale("log")
    ax.set_yscale("log")
    ax.set_title(r"Borůvka-OMP wall time vs graph size, $p \approx 4\,\ln(n)/n$")
    ax.legend()
    ax.grid(True, which="both", alpha=0.3)
    fig.tight_layout()
    fig.savefig(args.out, dpi=140)
    print(f"wrote {args.out}")


if __name__ == "__main__":
    main()
