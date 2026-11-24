# RHESSys testing

The tests that come with RHESSys, when each one runs, and how to run the ones you start yourself.

## Summary

| When it runs                                                      | Test                                             | What it checks                                                                                                              | How to run                                      |
| ----------------------------------------------------------------- | ------------------------------------------------ | --------------------------------------------------------------------------------------------------------------------------- | ----------------------------------------------- |
| **During the build** (opt-in)                                     | [[#C unit tests]]                                | Small functions and the output filter parser                                                                                | `make skiptests=F` in `rhessys/`                |
| **CI** (GitHub Actions, every pull request and push to `develop`) | [CI workflow](#ci)                               | Unit tests; the `w8_fast` case runs, gives the same result twice, and conserves water; no memory errors (ASan/UBSan)        | automatic                                       |
| **Manual**: before you merge a code change                        | [[#Regression comparison]]                       | Your change's outputs against the unchanged code's outputs, plus water balance (pass/fail) and carbon and nitrogen balances (report only) | `regress.sh baseline`, then `regress.sh check`  |
| **Manual**: quick checks, no reference needed                     | [[#Determinism and sanitizers]], `make functest` | Same result twice and water balance; memory errors and undefined behavior                                                   | `regress.sh determinism`, `regress.sh sanitize` |
| **Interactive**                                                   | [[#Interactive W8 example scripts]]              | Runs and plots the W8 test watershed in R; an introduction to running RHESSys                                               | open in RStudio / Jupyter                       |

`regress.sh` is `Testing/regression/regress.sh`, run from the repository root.

## C unit tests

- `rhessys/test/src/*.c`, built against the model objects and run by `make skiptests=F` in `rhessys/`.
- A plain `make` skips them (the default is `skiptests=T`).
- **Pass:** the build finishes. **Fail:** the build stops at the first failing test, with `not ok` in the output.

## CI

`.github/workflows/tests.yml` runs on every pull request and on pushes to `develop`:

1. Build with unit tests (`make skiptests=F`).
2. `regress.sh determinism w8_fast`: run the case twice; both runs must be bitwise identical and the water balance must pass.
3. `regress.sh sanitize w8_fast`: run with AddressSanitizer and UndefinedBehaviorSanitizer; any memory error or undefined behavior fails.

CI does not compare against reference outputs and does not build the fire model; run the [[#Regression comparison]] for that.

## Regression comparison

This is the main test for a code change. It runs a test case twice: once with the code before your change (the **baseline**), once with your change, and compares every output file.

### Requirements

- Linux or WSL (the harness uses GNU `time`), `bash`, `git`, `python3` (standard library only), and the RHESSys build dependencies (see the `apt-get install` line in `.github/workflows/tests.yml`).
- For `w8_fire_nc`: the WMFire library at `lib/libwmfire.so` (not in git; build it as in the Dockerfile).
- Work files go to `~/rhessys_regress` (set `RHESSYS_REGRESS_WORK` to change it). On WSL, keep it on the Linux filesystem; `/mnt/c` is much slower.

### Test cases

Both cases use the W8 watershed (HJ Andrews, `Testing/`).

| Case | Build | What it adds | Period | Run time |
|---|---|---|---|---|
| `w8_fast` | `default` | ASCII base-station climate | 5 water years, 1988-10-01 to 1993-10-01 | ~10 s |
| `w8_fire_nc` | `fire` (WMFire) | netCDF gridded climate, fire spread and effects (`-firespread 30`), ash | 10 water years, 1988-10-01 to 1998-10-01 | ~20 s |

Each case writes basin daily and patch yearly CSVs, basin daily carbon and nitrogen CSVs, and the end-of-run `.state` file; `w8_fire_nc` adds fire variables, a basin netCDF file and WMFire's `FireSizes_*.txt`. All of them are compared.

### Workflow

```
# 1. Make the reference from the code you started from
Testing/regression/regress.sh baseline w8_fast w8_fire_nc

# 2. Build your working tree, run, and compare
Testing/regression/regress.sh check w8_fast w8_fire_nc
```

1. **Pick the base** (the code to compare against). `baseline` builds a git commit in a temporary copy of the repository, so your working tree and build are not touched.
   - Your change is not committed yet: use the default, `HEAD`.
   - Your change is committed on a branch: name the commit it started from, for example `BASE_REF=develop regress.sh baseline w8_fast`, or `BASE_REF=$(git merge-base develop HEAD)`.
   - The base is built with that commit's makefile. If the makefile or compiler settings differ between the base and your branch, outputs can differ even when the model code didn't change.
   - The test inputs (case files, worldfile, climate) always come from your working tree, so both runs use the same inputs.
2. **Pick the cases.** Run `w8_fast` for every change. Add `w8_fire_nc` if you changed fire, ash, netCDF climate or WMFire code, or anything that fire calls.
3. **Run `check`.** It rebuilds your working tree, runs the case and compares it with the baseline. After more edits, run `check` again; the baseline stays until you replace it.
4. **Read the result** (below), then decide.

### Reading the result

`check` prints a report for each case and exits 0 when every case passes, 1 otherwise.

**Pass:** every output identical and the water balance closes.

```
== compare w8_fast: reference=/home/you/rhessys_regress/golden/w8_fast run=/home/you/rhessys_regress/runs/w8_fast
  9/9 files bitwise identical (tol_abs=0.0, tol_rel=0.0)
  RESULT: PASS
  water balance: PASS days=1825 max|daily|=5.00e-06 m on 1992-2-7 (limit 2e-05); cumulative=7.000e-05 m of input 5.504 m (limit 5.50e-03)
  carbon balance (report only): days=1825 cumulative residual=+0.0001 kgC/m2 (sum in 5.9442, sum out 8.2568, d(store) -2.3127); max|daily|=4.00e-06 on 1991-8-10
  nitrogen balance (report only): days=1825 cumulative residual=+0.030440 kgN/m2 (sum in 0.158775, sum out 0.104074, d(store) +0.024261); max|daily|=6.21e-04 on 1992-9-5
  INVARIANTS: PASS
```

**Fail: outputs differ.** Each differing CSV gets one line per column that changed: how many rows differ, the largest absolute and relative difference, and the first row that differs.

```
  5/9 files bitwise identical (tol_abs=0.0, tol_rel=0.0)
  DIFF: out/basin_carbon_daily.csv
    totalc            n_diff=1825   max_abs=1.305e+00 max_rel=9.895e-03 first_row=2
    net_plant_psn     n_diff=1238   max_abs=3.220e-04 max_rel=2.807e-01 first_row=209
  ...
  RESULT: FAIL
```

Other failure lines: `MISSING in test` / `EXTRA in test` (an output file appeared or disappeared), `NONFINITE` (new NaN or Inf values), `DIFF (binary)` (the netCDF file differs; there is no per-variable detail).

**Fail: water balance.** `water balance: FAIL` and `INVARIANTS: FAIL`; see [[#Water balance]].

### What to do with the result

- **PASS, and you expected no output change** (refactoring, speed-up, clean-up): done.
- **Outputs differ, and you expected no change:** your change affects the model. Use `first_row` and the variables listed to find where it starts; the runs are in `~/rhessys_regress/runs/<case>` (yours) and `~/rhessys_regress/golden/<case>` (the baseline).
- **Outputs differ, and you meant to change them** (a bug fix or new process): check that the variables that changed, and the size of the change, are what you expected, and describe them in your pull request.
- **Differences are tiny, from floating point** (for example a reordered sum): you can accept them within a tolerance you choose, `TOL_ABS=<value>` and/or `TOL_REL=<value>` before `check` or `compare`. A value counts as different only if it is beyond both. Files that differ only within tolerance are listed as `DIFF (within tolerance)` and the result is `PASS`. There is no built-in tolerance; the default is bitwise.
- **Water balance fails:** water is being created or lost, so treat it as a bug even if the outputs were meant to change.
- **Carbon or nitrogen balance changed:** neither fails the test. Compare the residuals with the baseline's (run `python3 Testing/regression/invariants.py ~/rhessys_regress/golden/<case>`); a residual that grew means carbon or nitrogen is being created or lost somewhere.

### Water balance

Pass/fail, from `out/basin_daily.csv` (basin averages, m of water). For each day:

```
residual = input − outputs − (storage today − storage yesterday)
```

| Role | Output variables (filter names) |
|---|---|
| Input | `patch.total_water_in` |
| Outputs | `patch.streamflow`, `hill.gw.Qout`, `patch.evaporation`, `patch.evaporation_surf`, `patch.transpiration_sat_zone`, `patch.transpiration_unsat_zone`, `patch.exfiltration_sat_zone`, `patch.exfiltration_unsat_zone` |
| Storage: soil | −(`patch.sat_deficit` − `patch.rz_storage` − `patch.unsat_storage`) |
| Storage: other | `patch.detention_store`, `patch.litter.rain_stored`, `patch.canopy_rain_stored`, `patch.canopy_snow_stored`, `patch.snowpack.water_equivalent_depth`, `hill.gw.storage` |

- **Pass:** every day |residual| ≤ 2e-5 m, and the total over the run ≤ 0.1% of total input. The limits allow for the 6 decimal places in the CSV; set them with `--max-daily` and `--max-cum-frac` when running `invariants.py` directly.
- In the CSV header the `patch.` / `hill.` prefixes are dropped (`gw.Qout`, `litter.rain_stored`).

### Carbon balance (report only)

From `out/basin_carbon_daily.csv` (basin averages, kgC/m²). Same form: residual = input − outputs − change in storage. It prints the total residual over the run and the worst day, and never fails.

| Role | Output variables (filter names) |
|---|---|
| Input | `patch.net_plant_psn` (photosynthesis minus maintenance and growth respiration, cover-weighted) |
| Outputs: heterotrophic respiration | `patch.cdf.litr1c_hr` to `litr4c_hr`, `patch.cdf.litr1c_hr_bg` to `litr4c_hr_bg`, `patch.cdf.soil1c_hr` to `soil4c_hr` |
| Outputs: other | `patch.streamflow_DOC`, `hill.gw.DOCout`, `patch.fire_c_veg_consumed` |
| Storage | `patch.totalc` (plants including coarse woody debris, litter and soil), `patch.soil_cs.DOC`, `patch.surface_DOC`, `patch.ash_DOC`, `hill.gw.DOC` |

What to expect on the current code:

- `w8_fast` closes: total residual about +0.0001 kgC/m² over 5 years.
- `w8_fire_nc` leaves about +0.28 kgC/m², all on fire days, because the litter carbon burned by fire is not an output variable yet.

### Nitrogen balance (report only)

From `out/basin_nitrogen_daily.csv` (basin averages, kgN/m²). Same form and output as the carbon balance; it never fails.

| Role | Output variables (filter names) |
|---|---|
| Inputs | `zone.ndep_NO3`, `zone.ndep_NH4` (deposition), `patch.ndf.nfix_to_sminn` (fixation), `patch.fertilizer_NO3_in`, `patch.fertilizer_NH4_in` |
| Outputs | `patch.ndf.denitrif`, `patch.streamflow_NO3`, `patch.streamflow_NH4`, `patch.streamflow_DON`, `hill.gw.NO3out`, `hill.gw.NH4out`, `hill.gw.DONout` |
| Storage: plants, litter, soil | `patch.totaln` (plants including coarse woody debris, litter, soil organic N, `nitrate` and `sminn`, the NH4 pool) |
| Storage: dissolved and surface | `patch.soil_ns.DON`, `patch.surface_NO3`, `patch.surface_NH4`, `patch.surface_DON`, `patch.litter.NO3_stored`, `stratum.NO3_stored` (canopy), `patch.fertilizer_NO3`, `patch.fertilizer_NH4` (fertilizer on the surface), `patch.ash_DON` |
| Storage: groundwater | `hill.gw.NO3`, `hill.gw.NH4`, `hill.gw.DON` |

What to expect on the current code:

- The balance does not close yet. `w8_fast` leaves +0.030 kgN/m² over 5 years, about 19% of the 0.159 kgN/m² deposited. Most of it is NO3 in the litter being overwritten each day (DEV_LOG #31); the rest is a smaller amount of N created, which is not located yet (#32).
- `w8_fire_nc` leaves +0.059 kgN/m²; fire days add residual because the nitrogen burned by fire is not an output variable yet (#34).
- Daily N fluxes are small (1e-6 to 1e-4 kgN/m²) and the CSV has 6 decimal places, so single days are coarse; the total over the run is the useful number.

### Determinism and sanitizers

Neither needs a baseline.

- `regress.sh determinism <case>`: runs the case twice with the same build. **Pass:** `RESULT: PASS` (both runs bitwise identical) and `INVARIANTS: PASS`. A difference means the model depends on something other than its inputs, such as uninitialized memory.
- `regress.sh sanitize <case>`: builds with AddressSanitizer and UndefinedBehaviorSanitizer and runs the case (about 3× slower). **Fail:** any `runtime error:` or `ERROR: AddressSanitizer` line. Memory leaks are listed but do not fail; they are structures not freed at exit.
- `make functest` (in `rhessys/`): builds and runs `determinism w8_fast`.

### All commands

```
regress.sh baseline <case>...     build BASE_REF (default HEAD) in a temporary worktree, run, save as the reference
regress.sh check <case>...        rebuild the working tree, run, compare with the reference
regress.sh build <config>...      build only (default | fire | asan | asan_fire); runs `make clean` in rhessys/
regress.sh run <case>...          run only, into ~/rhessys_regress/runs/<case>
regress.sh compare <case>...      compare the last run with the reference, without rerunning
regress.sh bless <case>...        save the last run as the reference (see below)
regress.sh determinism <case>...
regress.sh sanitize <case>...
regress.sh list                   list cases
```

| Setting | Meaning |
|---|---|
| `BASE_REF` | commit, branch or tag for `baseline` (default `HEAD`) |
| `TOL_ABS`, `TOL_REL` | tolerances for `check` / `compare` (default 0, bitwise) |
| `REBUILD=0` | `check` reuses the existing build instead of rebuilding |
| `RHESSYS_REGRESS_WORK` | work directory (default `~/rhessys_regress`) |
| `RHESSYS_GOLDEN` | reference directory (default `~/rhessys_regress/golden`) |
| `FORCE=1` | let `bless` or `baseline` replace a blessed reference (the old one is kept as `<case>.replaced.<time>`) |
| `RUN_WRAP` | prefix for the model command, for example `RUN_WRAP="valgrind --tool=memcheck"` |
| `RHESSYS_REGRESS_EXT` | a directory of extra cases and commands (`cases/*.case`, `regress_ext.sh`) kept outside the repository |

**Baseline or bless.** A `baseline` reference is temporary and is replaced by the next `baseline`. `bless` saves a run you want to keep as a fixed reference across many changes; it is not replaced unless you set `FORCE=1`. Reference outputs are bitwise results for one compiler and machine, so they are not kept in git.

**Run directory** (`~/rhessys_regress/runs/<case>`): `out/` (outputs), `rhessys.log`, `cmd.txt` (the command line), `run_info.txt` (build, git commit, run time and memory), `MANIFEST.sha256` (hashes of the compared files).

**Adding a case:** copy `Testing/regression/cases/w8_fast.case` and change the worldfile, dates, flags, tec and output filter (`Testing/regression/inputs/`). The filter must write `out/basin_daily.csv` (and `out/basin_carbon_daily.csv`, `out/basin_nitrogen_daily.csv`) with the variables above for the balance checks to run; without them they are skipped.

## Interactive W8 example scripts

`Testing/` has R scripts that run the W8 watershed and plot the results. They are a good introduction to running RHESSys and a place to explore a change visually; they are not pass/fail tests.

- `TestCase_IO.R`: runs W8 with `RHESSysIOinR` and plots the output. Run it with `Testing/` as the working directory.
- `TestCase.Rmd`, `TestCaseOf.Rmd`, `TestCase_new.ipynb`: the same test case as a walkthrough, comparing a run with saved earlier results (`W8TestCase.RData`).
- `run_RHESSysPreprocess_w8TC.R`: rebuilds the W8 worldfile and flow table with `RHESSysPreprocessing`.
