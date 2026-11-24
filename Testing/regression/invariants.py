#!/usr/bin/env python3
"""Physical invariant checks on a RHESSys regression run directory.

Checks:
  - basin water balance on out/basin_daily.csv (pass/fail; replaces the retired legacy
    rhessys/test/test_mass_balance.py check)
  - basin carbon balance on out/basin_carbon_daily.csv (REPORT ONLY, never fails; see below)
  - basin nitrogen balance on out/basin_nitrogen_daily.csv (REPORT ONLY, never fails; see below)

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

Carbon and nitrogen balances (REPORT ONLY, never fail): same form,
  residual(day) = inputs - outputs - d(storage), summed over the run.

Carbon, out/basin_carbon_daily.csv (kgC/m2). Closes to ~1e-4 on w8_fast since the #17 and #22
fixes (2026-11-20):
  inputs  = net_plant_psn                             (GPP - MR - GR, cover-weighted)
  outputs = heterotrophic resp (litr1-4 + litr1-4_bg + soil1-4 hr)
            + streamflow_DOC + gw.DOCout
            + fire_c_veg_consumed + fire.litter_c_consumed + fire.soil_c_consumed
  storage = totalc + soil_cs.DOC + surface_DOC + ash_DOC + gw.DOC
  patch.totalc = cover-weighted stratum totalc (plant + CWD) + soil + litter (incl. below ground).
  The fire consumed terms are gross and ash is storage, so fire days show -(ash added) as
  residual (DEV_LOG #37); w8_fire_nc also has an unexplained remainder on fire days (#36).

Nitrogen, out/basin_nitrogen_daily.csv (kgN/m2). Closes to ~1e-3 on the W8 cases since the #31 fix
(2026-11-24); the remainder is #32:
  inputs  = ndep_NO3 + ndep_NH4 + ndf.nfix_to_sminn + fertilizer_NO3_in + fertilizer_NH4_in
  outputs = ndf.denitrif + streamflow_NO3 + streamflow_NH4 + streamflow_DON
            + gw.NO3out + gw.NH4out + gw.DONout
            + fire_n_veg_consumed + fire.litter_n_consumed + fire.soil_n_consumed
  storage = totaln + soil_ns.DON + surface_NO3 + surface_NH4 + surface_DON + litter.NO3_stored
            + NO3_stored (canopy) + fertilizer_NO3 + fertilizer_NH4 (surface fertilizer)
            + ash_DON + gw.NO3 + gw.NH4 + gw.DON
  patch.totaln = cover-weighted stratum totaln (plant incl. npool/retransn + CWD) + soil organic
  + nitrate + sminn (NH4) + litter (incl. below ground). Fire days show -(ash N added) (#37).
  The fire columns (added 2026-11-24) are optional: if a filter lacks them they count as 0.
  Daily N fluxes are ~1e-6..1e-4 kgN/m2 and the CSV has 6 decimals, so single days are coarse;
  the cumulative residual is the useful number.

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
CARBON = dict(name="carbon", unit="kgC/m2", decimals=4,
              inputs=["net_plant_psn"],
              outputs=CARBON_HR + ["streamflow_DOC", "gw.DOCout", "fire_c_veg_consumed",
                                    "fire.litter_c_consumed", "fire.soil_c_consumed"],
              stores=["totalc", "soil_cs.DOC", "surface_DOC", "ash_DOC", "gw.DOC"])
NITROGEN = dict(name="nitrogen", unit="kgN/m2", decimals=6,
                inputs=["ndep_NO3", "ndep_NH4", "ndf.nfix_to_sminn", "fertilizer_NO3_in", "fertilizer_NH4_in"],
                outputs=["ndf.denitrif", "streamflow_NO3", "streamflow_NH4", "streamflow_DON",
                         "gw.NO3out", "gw.NH4out", "gw.DONout",
                         "fire_n_veg_consumed", "fire.litter_n_consumed", "fire.soil_n_consumed"],
                stores=["totaln", "soil_ns.DON", "surface_NO3", "surface_NH4", "surface_DON", "litter.NO3_stored",
                        "NO3_stored", "fertilizer_NO3", "fertilizer_NH4", "ash_DON", "gw.NO3", "gw.NH4", "gw.DON"])


# Fire terms added 2026-11-24; older filters and references without them still get a report.
OPTIONAL = {"fire.litter_c_consumed", "fire.soil_c_consumed",
            "fire_n_veg_consumed", "fire.litter_n_consumed", "fire.soil_n_consumed"}


def budget_report(path, spec):
    """Report-only basin budget: inputs - outputs - d(storage). Never fails the run."""
    label = f"{spec['name']} balance (report only)"
    with open(path) as f:
        rows = list(csv.DictReader(f))
    need = spec["inputs"] + spec["outputs"] + spec["stores"]
    missing = [c for c in need if rows and c not in rows[0]]
    if not rows or any(c not in OPTIONAL for c in missing):
        print(f"  {label}: SKIP ({os.path.basename(path)} missing columns {missing})")
        return
    note = f" [not in file, counted as 0: {', '.join(missing)}]" if missing else ""
    v = lambda x, cols: sum(float(x[c]) for c in cols if c in x)
    res, tin, tout, worst = 0.0, 0.0, 0.0, (0.0, None)
    for prev, x in zip(rows, rows[1:]):
        i, o = v(x, spec["inputs"]), v(x, spec["outputs"])
        r = i - o - (v(x, spec["stores"]) - v(prev, spec["stores"]))
        res += r; tin += i; tout += o
        if abs(r) > abs(worst[0]):
            worst = (r, f"{x.get('year')}-{x.get('month')}-{x.get('day')}")
    d = spec["decimals"]
    dstore = v(rows[-1], spec["stores"]) - v(rows[0], spec["stores"])
    print(f"  {label}: days={len(rows) - 1} cumulative residual={res:+.{d}f} {spec['unit']} "
          f"(sum in {tin:.{d}f}, sum out {tout:.{d}f}, d(store) {dstore:+.{d}f}); "
          f"max|daily|={abs(worst[0]):.2e} on {worst[1]}{note}")


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
    for fname, spec in (("basin_carbon_daily.csv", CARBON), ("basin_nitrogen_daily.csv", NITROGEN)):
        p = os.path.join(a.rundir, "out", fname)
        if os.path.exists(p):
            budget_report(p, spec)
    print("  INVARIANTS:", "PASS" if ok else "FAIL")
    return 0 if ok else 1


if __name__ == "__main__":
    sys.exit(main())
