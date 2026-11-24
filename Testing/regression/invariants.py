#!/usr/bin/env python3
"""Physical invariant checks on a RHESSys regression run directory.

Checks:
  - basin water balance on out/basin_daily.csv (pass/fail; replaces the retired legacy
    rhessys/test/test_mass_balance.py check)
  - basin carbon balance on out/basin_carbon_daily.csv (REPORT ONLY, never fails; see below)

  residual(day) = total_water_in
                  - (streamflow + gw.Qout + evaporation + evaporation_surf
                     + transpiration_sat_zone + transpiration_unsat_zone
                     + exfiltration_sat_zone + exfiltration_unsat_zone)
                  - d(storage)
  storage = -(sat_deficit - rz_storage - unsat_storage) + detention_store
            + litter.rain_stored + canopy_rain_stored + canopy_snow_stored
            + snowpack.water_equivalent_depth + gw.storage

Outputs are printed with 6 decimals, so the daily residual is limited by rounding
(~1e-6 per term). Defaults: max |daily residual| <= 2e-5 m and
|cumulative residual| <= 0.1% of cumulative input.

Carbon balance (report only). Before the #17 and #22 fixes (2026-11-20) it did not close; now
it closes to ~1e-4 kgC/m2 on w8_fast. Fire cases leave a residual (see below):

  residual(day) = net_plant_psn                       (GPP - MR - GR, cover-weighted)
                  - heterotrophic resp                (litr1-4 + litr1-4_bg + soil1-4 hr)
                  - streamflow_DOC - gw.DOCout - fire_c_veg_consumed
                  - d(totalc + soil_cs.DOC + surface_DOC + ash_DOC + gw.DOC)
  patch.totalc = cover-weighted stratum totalc (plant + CWD) + soil + litter (incl. below ground).

  fire.litter_c_consumed is not output-filterable,
  so fire days show the litter consumed (and ash double counting) as residual.

Exit 0 = pass (or check not applicable: required columns missing), 1 = fail.
"""
import argparse
import csv
import os
import sys

FLUX_OUT = ["streamflow", "gw.Qout", "evaporation", "evaporation_surf", "transpiration_sat_zone",
            "transpiration_unsat_zone", "exfiltration_sat_zone", "exfiltration_unsat_zone"]
STORE_POS = ["detention_store", "litter.rain_stored", "canopy_rain_stored", "canopy_snow_stored",
             "snowpack.water_equivalent_depth", "gw.storage"]
SAT = ["sat_deficit", "rz_storage", "unsat_storage"]
REQUIRED = ["total_water_in"] + FLUX_OUT + STORE_POS + SAT


def water_balance(path, max_daily, max_cum_frac):
    with open(path) as f:
        rows = list(csv.DictReader(f))
    if not rows or any(c not in rows[0] for c in REQUIRED):
        missing = [c for c in REQUIRED if rows and c not in rows[0]]
        print(f"  water balance: SKIP ({os.path.basename(path)} missing columns {missing})")
        return True

    def store(x):
        sd = float(x["sat_deficit"]) - float(x["rz_storage"]) - float(x["unsat_storage"])
        return -sd + sum(float(x[c]) for c in STORE_POS)

    res = []
    total_in = 0.0
    worst = (0.0, None)
    for prev, x in zip(rows, rows[1:]):
        inflow = float(x["total_water_in"])
        total_in += inflow
        r = inflow - sum(float(x[c]) for c in FLUX_OUT) - (store(x) - store(prev))
        res.append(r)
        if abs(r) > abs(worst[0]):
            worst = (r, f"{x.get('year')}-{x.get('month')}-{x.get('day')}")
    cum = sum(res)
    cum_lim = max_cum_frac * total_in
    ok = abs(worst[0]) <= max_daily and abs(cum) <= cum_lim
    print(f"  water balance: {'PASS' if ok else 'FAIL'} days={len(res)} max|daily|={abs(worst[0]):.2e} m on {worst[1]} "
          f"(limit {max_daily:.0e}); cumulative={cum:.3e} m of input {total_in:.3f} m (limit {cum_lim:.2e})")
    return ok


CARBON_HR = ["cdf.litr1c_hr", "cdf.litr2c_hr", "cdf.litr3c_hr", "cdf.litr4c_hr", "cdf.litr1c_hr_bg",
             "cdf.litr2c_hr_bg", "cdf.litr3c_hr_bg", "cdf.litr4c_hr_bg", "cdf.soil1c_hr", "cdf.soil2c_hr", "cdf.soil3c_hr", "cdf.soil4c_hr"]
CARBON_OUT = ["streamflow_DOC", "gw.DOCout", "fire_c_veg_consumed"]
CARBON_STORE = ["totalc", "soil_cs.DOC", "surface_DOC", "ash_DOC", "gw.DOC"]
CARBON_REQUIRED = ["net_plant_psn"] + CARBON_HR + CARBON_OUT + CARBON_STORE


def carbon_balance(path):
    """Report-only basin carbon budget. Returns nothing; never fails the run."""
    with open(path) as f:
        rows = list(csv.DictReader(f))
    if not rows or any(c not in rows[0] for c in CARBON_REQUIRED):
        missing = [c for c in CARBON_REQUIRED if rows and c not in rows[0]]
        print(f"  carbon balance (report only): SKIP ({os.path.basename(path)} missing columns {missing})")
        return
    v = lambda x, c: float(x[c])
    store = lambda x: sum(v(x, c) for c in CARBON_STORE)
    res, npp, hr, worst = 0.0, 0.0, 0.0, (0.0, None)
    for prev, x in zip(rows, rows[1:]):
        h = sum(v(x, c) for c in CARBON_HR)
        r = v(x, "net_plant_psn") - h - sum(v(x, c) for c in CARBON_OUT) - (store(x) - store(prev))
        res += r; npp += v(x, "net_plant_psn"); hr += h
        if abs(r) > abs(worst[0]):
            worst = (r, f"{x.get('year')}-{x.get('month')}-{x.get('day')}")
    ndays = len(rows) - 1
    print(f"  carbon balance (report only): days={ndays} cumulative residual={res:+.4f} kgC/m2 "
          f"(sum net_psn {npp:.4f}, sum hetero resp {hr:.4f}, d(store) {store(rows[-1]) - store(rows[0]):+.4f}); "
          f"max|daily|={abs(worst[0]):.2e} on {worst[1]}")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("rundir")
    ap.add_argument("--max-daily", type=float, default=2e-5)
    ap.add_argument("--max-cum-frac", type=float, default=1e-3)
    a = ap.parse_args()
    ok = True
    p = os.path.join(a.rundir, "out", "basin_daily.csv")
    if os.path.exists(p):
        ok = water_balance(p, a.max_daily, a.max_cum_frac) and ok
    else:
        print("  water balance: SKIP (no out/basin_daily.csv)")
    p = os.path.join(a.rundir, "out", "basin_carbon_daily.csv")
    if os.path.exists(p):
        carbon_balance(p)
    print("  INVARIANTS:", "PASS" if ok else "FAIL")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
