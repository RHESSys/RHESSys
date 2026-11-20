# RHESSys dev log

Findings and changes from the automated dev effort. For each finding: location, category, evidence, whether fixing it **changes outputs**, and status.

## Changes

| Date | Change | Changes outputs? | Verified by |
|---|---|---|---|
| 2026-11-19 | `rhessys/makefile`: new `LTO_FLAGS` variable, also added to `CFLAGS_TESTS`, so the unit tests link against the `-flto` objects. Before this, `skiptests=F` failed at link time ("file format not recognized"). | no (binary build flags unchanged) | 16/16 unit tests pass |
| 2026-11-19 | `rhessys/makefile`: the `test` rule now runs `$(TESTS)` with `&&`, so a failing or crashing test fails the build. The old rule used `cd test; ./x; cd -;`, which ignored exit codes, and it took its test list (`TESTS_TO_RUN`) from a `find` at parse time. | no | a segfaulting test used to pass silently |
| 2026-11-19 | `rhessys/test/src/test_output_filter_patch_list.c`: rewritten to set the patch IDs explicitly and to test de-duplication. The old test assumed no de-duplication and read uninitialized IDs, so it segfaulted. | no (test only) | test passes |
| 2026-11-19 | W8 test inputs: netcdf clim, fire grids, `fire.def` dims, `fire_parm_ID` in the worldfile. Regression harness and golden outputs added (see TESTING.md). | no | determinism + bitwise checks |

## Findings (candidates for step 2/3)

| # | Location | Category | Finding | Changes outputs? | Status |
|---|---|---|---|---|---|
| 1 | `output_filter/output_filter.c` `create_new_output_filter_patch` | stability | Uses `malloc`, so the type and IDs are left uninitialized. Callers have to set every field before calling `add_to_output_filter_patch_list`, which compares IDs. | probably not (the parser sets the fields) | open |
| 2 | fire output (`hydro/zero_patch_daily_flux.c`, fire effects) | correctness / output | In `w8_fire_nc`:<br>• basin-daily `patch.burn`, `patch.nburn`, `patch.fire_c_veg_consumed`, `patch.fire_c_veg_mortality` and `patch.ash_DOC/DON` are always 0, even though 5 fires happen and carbon is removed at the month boundary.<br>• yearly `fire_c_veg_consumed/mortality` are also 0.<br>• yearly `burn` reaches 352 for a patch-year, which is odd for a 0-1 fraction.<br>The fluxes look like they're zeroed (commit fe4f3eb8) before output or accumulation can see them. | yes (output only) | open, Phase A candidate |
| 3 | `FIRE/makefile` | build / portability | Doesn't build on Linux: objects are compiled without `-fPIC`, and `BOOST_ROOT` is hard-coded. Docker builds with boost `b2` instead, so `libwmfire.so` can't be rebuilt locally without b2. | no | open |
| 4 | `travis.sh` | tests | Stale: it does `cd cf`, but cf has moved to `util/deprecated/cf`. Nothing calls it. | no | open (step 1) |
| 5 | `rhessys/test/test_mass_balance.py` | tests | Uses `errno` without importing it. Its W8.zip test has an old command line. | no | open (step 1) |
| 6 | `rhessys/makefile` `TESTS_TO_RUN` | build | Still used by `clean`. It is evaluated at parse time, so it only sees binaries that already exist. | no | open (minor) |
| 7 | output filters, yearly/monthly | docs | Yearly/monthly filters only accept `accumulate_patch_object` fields (e.g. `patch.et`, `patch.psn`), not the daily field names. | no | note |
| 8 | `RHESSysPreprocessing::write_fire_grids` (separate repo) | tool bug | With unsigned-int rasters, NA is written as 4294957297 instead of -9999. | no (RHESSys) | open, outside this repo |
| 9 | Ward project | FYI | Ward's run hdrs use `clim/ward_netcdfgridmet_agg.base`, which is ASCII clim made from gridmet, not `-netcdfgrid`. The W8 `w8_fire_nc` case is currently the only netcdf-input regression case. | - | note |
