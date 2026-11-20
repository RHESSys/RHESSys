# RHESSys testing

How RHESSys is tested, what each test covers, and how to run it. The dev plan and its decisions are kept outside this repo; findings and changes go in [DEV_LOG.md](DEV_LOG.md).

## Summary

| Test | Type | How to run | Runtime | Automated | Status |
|---|---|---|---|---|---|
| C unit tests (`rhessys/test/src`, 16 tests) | unit (glib) | `make skiptests=F` in `rhessys/` | ~1 min (with build) | runs during the build; a failure fails the build | passing |
| Regression: `w8_fast` | regression, bitwise | `Testing/regression/regress.sh check w8_fast` | ~8 s run | script | golden blessed |
| Regression: `w8_fire_nc` | regression, bitwise | `Testing/regression/regress.sh check w8_fire_nc` | ~16 s run | script | golden blessed |
| Regression: Ward long case | regression | not defined yet | long | - | to do |
| Python functional test (`rhessys/test/test_mass_balance.py`, `make functest`) | functional | `make functest` | ? | no | stale: `errno` is used without being imported; not checked against the current CLI |
| `travis.sh` | build + tests | - | - | - | stale: it does `cd cf`, but cf now lives in `util/deprecated/cf` |
| `Testing/TestCase*.Rmd`, `TestCase_IO.R`, `TestCase_new.ipynb` | manual R workflow / plots | interactive | `TestCase_IO.R` runs 1988-2189 | no | manual |
| RHESSys calibration test kit (`Projects/CodeDev/rhessys_calibration_tests`) | R package / calibration scripts | `./run_tests.sh` in Git Bash | ~6 min | script (outside this repo) | used for RHESSysIOinR / rhutils / calibration script changes |
| GitHub Actions (`.github/workflows`) | Docker image build only | on push | - | yes | no tests run in CI yet |

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
regress.sh list
```

- `WORK` = `$RHESSYS_REGRESS_WORK`, default `~/rhessys_regress`. Use the WSL filesystem, which is much faster than `/mnt/c`.
- `GOLDEN` = `$RHESSYS_GOLDEN`, default `<repo>/../RHESSys-regression-golden`. Golden outputs stay local and are not in git.
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

### Cases

| Case | Build | Climate | Fire | Period | Outputs |
|---|---|---|---|---|---|
| `w8_fast` | default | ASCII base station `clim/w8_base` | no | 1988-10-01 .. 1993-10-01 | basin daily csv (50 vars), patch yearly csv (23 vars), state |
| `w8_fire_nc` | fire | netcdf `clim/w8_netcdf.base` (`-netcdfgrid`) | `-firespread 30`, `defs/fire.def`, `fire_grid/w8_fire30m.*` | 1988-10-01 .. 1998-10-01 | the above plus fire vars, basin daily netcdf, FireSizes |

Both cases use the command-line parameters from `TestCase_IO.R` (`-g -s/-sv/-svalt/-gw ...`). `w8_fire_nc` has 5 fires in 10 years (sizes 7-48 cells, with `ran_seed 1`), and fire effects remove/transfer carbon at the month boundary after each one.

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

Golden outputs were blessed on 2026-11-19 from commit `80e63a72` (branch `develop-test-perf`). The only local changes were to the makefile and a unit test, with no model code changes. Each golden case's `run_info.txt` records the commit, diff hash, binary sha256 and compiler. Both cases pass the run-twice determinism check, netcdf output included.
