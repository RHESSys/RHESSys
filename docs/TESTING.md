# RHESSys testing

How RHESSys is tested, what each test covers, and how to run it. The dev plan and its decisions are kept outside this repo; findings and changes go in [DEV_LOG.md](DEV_LOG.md).

## Summary

| Test                                                                          | Type                                                       | How to run                                       | Runtime                        | Automated                                        | Status                                                                                                                                                                 |
| ----------------------------------------------------------------------------- | ---------------------------------------------------------- | ------------------------------------------------ | ------------------------------ | ------------------------------------------------ | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| C unit tests (`rhessys/test/src`, 16 tests)                                   | unit (glib)                                                | `make skiptests=F` in `rhessys/`                 | ~1 min (with build)            | runs during the build; a failure fails the build | passing                                                                                                                                                                |
| Regression: `w8_fast`                                                         | regression, bitwise                                        | `Testing/regression/regress.sh check w8_fast`    | ~8 s run                       | script                                           | golden blessed                                                                                                                                                         |
| Regression: `w8_fire_nc`                                                      | regression, bitwise                                        | `Testing/regression/regress.sh check w8_fire_nc` | ~16 s run                      | script                                           | golden blessed                                                                                                                                                         |
| Regression: `ward_1y`                                                         | regression, bitwise (large basin, 12,045 patches)          | `regress.sh check ward_1y`                       | ~3 min                         | script                                           | golden blessed                                                                                                                                                         |
| Regression: `ward_long`                                                       | regression, bitwise (Ward full climate record)             | `regress.sh check ward_long`                     | 1 h 47 m (608 MB RSS)          | script (nightly / on demand)                     | golden blessed 2026-11-19, re-blessed 2026-11-20 with the carbon output added (existing 11 files unchanged); determinism verified (2 runs of the same binary, 11/11 files bitwise identical, 1 h 52 m)                                                   |
| Water-balance invariant (`invariants.py`)                                     | invariant                                                  | runs inside `regress.sh compare`/`check`         | seconds                        | script                                           | passing (all cases)                                                                                                                                                    |
| Carbon-balance report (`invariants.py`) | invariant, **report only** | runs inside `regress.sh compare`/`check` (needs `out/basin_carbon_daily.csv`) | seconds | script | never fails. Known not to close: DEV_LOG #17 (`mr_deficit` repayment double counted) and #18 |
| Sanitizers: ASan + UBSan                                                      | memory / UB                                                | `regress.sh sanitize <case>`                     | ~3x a normal run               | script, CI                                       | clean: `w8_fast`, `w8_fire_nc`, `ward_1y` (leaks at exit only)                                                                                                         |
| valgrind memcheck (incl. uninitialized reads)                                 | memory                                                     | manual (see DEV_LOG)                             | ~50x                           | no                                               | clean: `w8_fast` (0 errors)                                                                                                                                            |
| Coverage (clang source-based)                                                 | coverage                                                   | `regress.sh coverage <case>...`                  | ~2-3x a normal run             | script                                           | 45% lines (W8)                                                                                                                                                         |
| `make functest` | functional | `make functest` | ~10 s | script | now runs `regress.sh check w8_fast`. The legacy Python test (`test_mass_balance.py`, `test/data/W8.zip`) was retired 2026-11-20; its only real check (basin water balance) is covered by `invariants.py` |
| `travis.sh`                                                                   | build + tests                                              | `./travis.sh`                                    | ~2 min                         | script (local)                                   | unit tests + `w8_fast` determinism + invariants. Travis CI itself is disabled (`.travis.yml.disabled`, see the comments there to re-enable); CI is GitHub Actions only |
| `Testing/TestCase*.Rmd`, `TestCase_IO.R`, `TestCase_new.ipynb`                | manual R workflow / plots                                  | interactive                                      | `TestCase_IO.R` runs 1988-2189 | no                                               | manual                                                                                                                                                                 |
| RHESSys calibration test kit (`Projects/CodeDev/rhessys_calibration_tests`)   | R package / calibration scripts                            | `./run_tests.sh` in Git Bash                     | ~6 min                         | script (outside this repo)                       | used for RHESSysIOinR / rhutils / calibration script changes                                                                                                           |
| GitHub Actions: `build_docker_*.yml`                                          | Docker image build                                         | on push                                          | -                              | yes                                              | unchanged                                                                                                                                                              |
| GitHub Actions: `tests.yml`                                                   | unit tests, `w8_fast` determinism + invariants, ASan/UBSan | PR, push to develop(-test-perf)                  | ~5 min                         | yes (once pushed)                                | new; no golden comparison or fire build in CI                                                                                                                          |

## C unit tests

- `make skiptests=F` builds each `rhessys/test/src/*.c` against all the model objects except `main.o`, then runs it. The `test` rule stops at the first failing test and fails the build.
- Default is `skiptests=T`, so a plain `make` skips them.
- Every link step that uses the model objects needs `$(LTO_FLAGS)`, because objects built with `-flto` are LLVM bitcode.

## Regression harness (`Testing/regression/`)

```
regress.sh build [default|fire]   # make clean + make; binary -> $WORK/bin/<config>/rhessys (+ build_info.txt)
regress.sh run <case>             # run into $WORK/runs/<case>; writes MANIFEST.sha256 + run_info.txt (wall time, max RSS)
regress.sh compare <case>         # compare the run against the golden copy (compare.py)
regress.sh check <case>           # build if needed, then run + compare
regress.sh determinism <case>     # run twice, compare the two runs
regress.sh bless <case>           # copy the run to golden (FORCE=1 to replace; the old copy is kept as .replaced.<time>)
regress.sh sanitize <case>...     # run with the asan / asan_fire build; memory errors/UB fail, leaks are reported
regress.sh coverage <case>...     # run with the coverage builds; report in $WORK/coverage/report.txt
BUILD=<config> regress.sh run <case>   # run any case with another build config
regress.sh list
```

- `WORK` = `$RHESSYS_REGRESS_WORK`, default `~/rhessys_regress`. Use the WSL filesystem, which is much faster than `/mnt/c`.
- `GOLDEN` = `$RHESSYS_GOLDEN`, default `<repo>/../RHESSys-regression-golden`. Golden outputs stay local and are not in git.
- `INPUTS` = `$RHESSYS_REGRESS_INPUTS`, default `<repo>/../RHESSys-regression-inputs`. This holds snapshots of large external inputs (Ward), referenced by `CASE_ROOT` in the case files.
- `build` runs `make clean` in `rhessys/`.
- Build configs:
  - `default` = `skiptests=F` (netcdf=T, wmfire=F)
  - `fire` = `skiptests=F wmfire=T`. This uses the existing `lib/libwmfire.so`, whose sha256 is recorded in `build_info.txt`. WMFire is built with boost `b2` (see the Dockerfile); `FIRE/makefile` does not work as-is.
- Run dir layout: `defs`, `clim`, `flowtables` and `fire_grid` are symlinked from `Testing/`. The worldfile, hdr, tec and filter are copied into the run dir, so `.state` output lands in the run dir and never in the repo.
- Outputs compared: every file the run writes, which covers:
  - filter outputs (`out/*.csv`, `out/*.nc`)
  - the end-of-run `.state` file (full model state)
  - the `_*.params` echo files
  - WMFire's `FireSizes_*.txt`

### Comparison (`compare.py`)

- Files with equal sha256 are identical.
- Text files that differ are compared token by token. CSVs get a per-column report (`n_diff`, `max_abs`, `max_rel`, first row).
- Binary files (netcdf) are reported only as differing.
- Fails on: missing or extra files, any difference beyond tolerance, and new NaN/Inf values that the reference doesn't have.
- Default tolerance is 0 (bitwise). Use `TOL_ABS` / `TOL_REL` only for changes approved as tolerance changes. Note that CSV/state values are printed to about 6 significant figures, so tiny floating-point changes may not show up in them.

### Profiling, benchmark, OpenMP

- `regress.sh profile <case>`: callgrind run with the `profile`/`profile_fire` build (`-O2 -flto -g`, outputs bitwise identical to `default`). Annotated self/inclusive costs go to `$WORK/profile/`. The `ward_1m` case (Ward, 1 month, 22 s) exists for this: callgrind on it takes about 13 min.
- `regress.sh bench <case>`: median wall time of `BENCH_N` (default 3) runs with the case's build. `BUILD=openmp OMP_NUM_THREADS=N` benchmarks the OpenMP build.
- `openmp` build config: `ward_1m` is bitwise identical to the serial build at 1 and 3 threads (2026-11-20).
- `RUN_WRAP="valgrind --tool=memcheck ..."` runs any case under a wrapper.

### Invariants (`invariants.py`)

Run by `regress.sh compare`/`check` on every case:

- **Water balance** (pass/fail): basin daily residual of input − outputs − Δstorage from `out/basin_daily.csv`. Limits: every day ≤ 2e-5 m and cumulative ≤ 0.1% of input.
- **Carbon balance** (report only, added 2026-11-20): from `out/basin_carbon_daily.csv` (a second filter block in each case's yml). Residual = `net_plant_psn` − heterotrophic respiration − `streamflow_DOC` − `gw.DOCout` − `fire_c_veg_consumed` − Δ(`totalc` + soil, surface, ash and gw DOC). It prints the cumulative residual and the worst day. It is still report only (fire cases don't close). Results on the current goldens:

  | Case | Before fixes (kgC/m²) | After #17 and #22 (2026-11-20) | Notes |
  |---|---|---|---|
  | `w8_fast` (5 yr) | −1.327 | +0.0001 | closes; max daily 4e-06 |
  | `w8_fire_nc` (10 yr) | −2.131 | +0.276 | remainder is on fire days: fire litter consumption can't be output-filtered (`fire.litter_c_consumed`) |
  | `ward_1y` | −0.072 | −0.002 | max daily 1e-03 on one day (1979-10-1) |
  | `ward_long` (43.6 yr) | −1.309 | −0.041 | same worst day as `ward_1y` |

  Before the fixes, 99.8% of the `w8_fast` residual was `mr_deficit` repayment (#17).

- Nitrogen balance: not checked yet (expected to fail; known issue).

### Cases

| Case         | Build   | Climate                                      | Fire                                                                                                | Period                                 | Outputs                                                      |
| ------------ | ------- | -------------------------------------------- | --------------------------------------------------------------------------------------------------- | -------------------------------------- | ------------------------------------------------------------ |
| `w8_fast`    | default | ASCII base station `clim/w8_base`            | no                                                                                                  | 1988-10-01 .. 1993-10-01               | basin daily csv (50 vars), patch yearly csv (23 vars), state |
| `w8_fire_nc` | fire    | netcdf `clim/w8_netcdf.base` (`-netcdfgrid`) | `-firespread 30`, `defs/fire.def`, `fire_grid/w8_fire30m.*`; ash on (`defs/soil_sandyloam_ash.def`) | 1988-10-01 .. 1998-10-01               | the above plus fire vars, basin daily netcdf, FireSizes      |
| `ward_1y`    | default | Ward ASCII gridmet clim (+10% rain)          | no                                                                                                  | 1979-01-01 .. 1979-12-30               | basin daily csv, state (70 MB)                               |
| `ward_long`  | default | same                                         | no                                                                                                  | 1979-01-01 .. 2022-08-07 (full record) | basin daily csv, state                                       |

The Ward cases use the best calibrated run as of 2026-11-19, **RunID296_345** (composite 0.305, NSE 0.45, KGE 0.50), with `-g -vmort_off`. The snapshot reproduces the calibration batch exactly: `ward_long` basin streamflow is bitwise identical to `output/CAL_RunID296_.../Wardsoil_cal_basin_RunID296_345.csv` on all 14,244 days.
- Inputs are a snapshot made by `Testing/regression/ward/make_ward_best_inputs.R`. It copies the base inputs and rebuilds the defs from the archived parameter values with `RHESSysIOinR::change_def_file`, because the per-ParSetID def copies in Ward are overwritten by every batch.
- Provenance: `$INPUTS/ward_best/SNAPSHOT_INFO.txt`.
- The Ward project itself is only read, never written.
- The last output day is the day before `-ed` (see DEV_LOG #15).

The W8 cases use the command-line parameters from `TestCase_IO.R` (`-g -s/-sv/-svalt/-gw ...`). `w8_fire_nc` has 5 fires in 10 years (sizes 7-48 cells, with `ran_seed 1`), and fire effects remove/transfer carbon at the month boundary after each one.

### W8 inputs added for these cases

- `Testing/clim/make_w8_netcdf.R`: builds `w8_netcdf_{tmax,tmin,rain}.nc` and `w8_netcdf.base` from the ASCII W8 clim, using the Ward gridmet layout:
  - dims lon×lat×time, with 2 lon cells
  - time as int days since 1900
  - temperature in C, precip in mm × 0.001

  An ASCII run and a netcdf run of the same model differ only by input float rounding (total C rel diff ≤ 1.3e-5), which confirms the date and unit alignment.
- `Testing/fire_grid/w8_fire30m.{dem,hillslope,zone,patch}`: a 28×18 grid at 30 m, made with `RHESSysPreprocessing::write_fire_grids` on the W8 template and rasters. The NA values it wrote (4294957297, from the unsigned rasters) were replaced with -9999.
- `Testing/defs/fire.def`: `n_rows 28`, `n_cols 18`.
- `Testing/worldfiles/w8TC.world`: `1 fire_parm_ID` added under every `landuse_parm_ID`. Non-fire outputs are bitwise identical with and without it.

### Baseline

Golden outputs were first blessed on 2026-11-19 from commit `80e63a72`. `w8_fire_nc` was re-blessed after the fire output fix (see DEV_LOG); `w8_fast` was unaffected. **All four cases were re-blessed on 2026-11-20 on branch `develop-fixes` after the model fixes #17, #21 and #22** (commit `a3fa6141`); the previous goldens are kept as `<case>.replaced.*` in the golden dir.

Original blessing: 2026-11-19 from commit `80e63a72` (branch `develop-test-perf`). The only local changes were to the makefile and a unit test, with no model code changes. Each golden case's `run_info.txt` records the commit, diff hash, binary sha256 and compiler. Both cases pass the run-twice determinism check, netcdf output included.
