#!/usr/bin/env python3
"""Compare two RHESSys regression run directories (reference vs test).

Each directory holds a MANIFEST.sha256 written by regress.sh. Files with equal hashes
are identical. For differing files:
  - text files (csv, state, fire outputs): compared token by token; numeric tokens
    are compared with abs/rel tolerance, others must match exactly. For CSVs the
    report is per column (max abs diff, max rel diff, first differing row).
  - binary files (netcdf): reported as differing (no numeric comparison yet).
Also flags NaN/Inf values in test text outputs that are not present in the reference.

Exit status 0 = pass (identical, or all differences within tolerance), 1 = fail.
Default tolerance is 0, i.e. any difference fails.
"""
import argparse
import math
import os
import re
import sys

NUM_SPLIT = re.compile(r"[,\s]+")


def read_manifest(d):
    m = {}
    with open(os.path.join(d, "MANIFEST.sha256")) as f:
        for line in f:
            h, name = line.rstrip("\n").split(None, 1)
            m[name.lstrip("*")] = h
    return m


def is_text(path):
    with open(path, "rb") as f:
        chunk = f.read(4096)
    return b"\0" not in chunk


def to_float(tok):
    try:
        return float(tok)
    except ValueError:
        return None


def nonfinite_count(path):
    n = 0
    with open(path) as f:
        for line in f:
            for tok in NUM_SPLIT.split(line.strip()):
                v = to_float(tok)
                if v is not None and not math.isfinite(v):
                    n += 1
    return n


def diff_csv(ref, test, tol_abs, tol_rel, out):
    with open(ref) as fr, open(test) as ft:
        hr = fr.readline().strip().split(",")
        ht = ft.readline().strip().split(",")
        if hr != ht:
            out.append(f"    header differs: ref has {len(hr)} cols, test has {len(ht)}")
            missing = [c for c in hr if c not in ht]
            extra = [c for c in ht if c not in hr]
            if missing:
                out.append(f"      missing in test: {missing[:10]}")
            if extra:
                out.append(f"      extra in test: {extra[:10]}")
            return False
        stats = {c: [0.0, 0.0, None, 0] for c in hr}  # max_abs, max_rel, first_row, n_diff
        nrow = 0
        ok = True
        for lr, lt in zip(fr, ft):
            nrow += 1
            vr = lr.rstrip("\n").split(",")
            vt = lt.rstrip("\n").split(",")
            for c, a, b in zip(hr, vr, vt):
                if a == b:
                    continue
                fa, fb = to_float(a), to_float(b)
                s = stats[c]
                if fa is None or fb is None:
                    s[3] += 1
                    s[2] = s[2] or nrow
                    ok = False
                    continue
                d = abs(fa - fb)
                r = d / abs(fa) if fa != 0 else (math.inf if d > 0 else 0.0)
                s[0] = max(s[0], d)
                s[1] = max(s[1], r)
                s[3] += 1
                s[2] = s[2] or nrow
                if d > tol_abs and r > tol_rel:
                    ok = False
        rest_r, rest_t = fr.read(), ft.read()
        if rest_r or rest_t:
            out.append(f"    row count differs after {nrow} rows")
            ok = False
    for c, (ma, mr, first, n) in stats.items():
        if n:
            out.append(f"    {c:<45s} n_diff={n:<6d} max_abs={ma:.3e} max_rel={mr:.3e} first_row={first}")
    return ok


def diff_text(ref, test, tol_abs, tol_rel, out):
    with open(ref) as fr, open(test) as ft:
        tr = NUM_SPLIT.split(fr.read().strip())
        tt = NUM_SPLIT.split(ft.read().strip())
    if len(tr) != len(tt):
        out.append(f"    token count differs: ref {len(tr)} vs test {len(tt)}")
        return False
    n = 0
    ma = mr = 0.0
    first = None
    ok = True
    for i, (a, b) in enumerate(zip(tr, tt)):
        if a == b:
            continue
        n += 1
        first = first if first is not None else i
        fa, fb = to_float(a), to_float(b)
        if fa is None or fb is None:
            ok = False
            continue
        d = abs(fa - fb)
        r = d / abs(fa) if fa != 0 else (math.inf if d > 0 else 0.0)
        ma, mr = max(ma, d), max(mr, r)
        if d > tol_abs and r > tol_rel:
            ok = False
    out.append(f"    tokens differing={n} max_abs={ma:.3e} max_rel={mr:.3e} first_token={first}")
    return ok


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("ref")
    ap.add_argument("test")
    ap.add_argument("--tol-abs", type=float, default=0.0)
    ap.add_argument("--tol-rel", type=float, default=0.0)
    a = ap.parse_args()

    mr, mt = read_manifest(a.ref), read_manifest(a.test)
    ok = True
    lines = []
    for name in sorted(set(mr) - set(mt)):
        lines.append(f"  MISSING in test: {name}")
        ok = False
    for name in sorted(set(mt) - set(mr)):
        lines.append(f"  EXTRA in test:   {name}")
        ok = False
    n_same = 0
    for name in sorted(set(mr) & set(mt)):
        pt = os.path.join(a.test, name)
        if is_text(pt):
            nf_t = nonfinite_count(pt)
            nf_r = nonfinite_count(os.path.join(a.ref, name))
            if nf_t > nf_r:
                lines.append(f"  NONFINITE: {name} has {nf_t} NaN/Inf values (reference: {nf_r})")
                ok = False
        if mr[name] == mt[name]:
            n_same += 1
            continue
        pr = os.path.join(a.ref, name)
        detail = []
        if not (is_text(pr) and is_text(pt)):
            lines.append(f"  DIFF (binary): {name}")
            ok = False
            continue
        within = (diff_csv if name.endswith(".csv") else diff_text)(pr, pt, a.tol_abs, a.tol_rel, detail)
        tag = "DIFF (within tolerance)" if within else "DIFF"
        lines.append(f"  {tag}: {name}")
        lines.extend(detail)
        ok = ok and within
    print(f"  {n_same}/{len(mr)} files bitwise identical (tol_abs={a.tol_abs}, tol_rel={a.tol_rel})")
    for l in lines:
        print(l)
    print("  RESULT:", "PASS" if ok else "FAIL")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
