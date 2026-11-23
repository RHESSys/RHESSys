# Unbuilt source files: pending removal

These 35 `.c` files were removed from the source tree on 2026-11-20 (commit `5586dabe`,
DEV_LOG finding #29). They are kept here, at their original relative paths, for a while,
so anyone who still needs one can claim it before they are deleted for good.

- **Not compiled.** The makefile never listed them, and this folder is outside the
  directories it scans. Several no longer compile against the current headers.
- **What they are:**
  - `output/output_csv_*` (22) and `output/add_csv_*headers` (2): the old CSV output writers
  - `tec/execute_csv_*_output_event` (5): the tec events that called those writers
  - `init/construct_WUI_listNew.c`, `init/construct_WUI_listByFamily.c`,
    `init/construct_dclim.c`, `init/construct_fire_gridLegacy.c`,
    `init/construct_patch_conv.c`, `util/read_basin.c`: superseded or unfinished versions
- **To keep one:** move it back to its original directory under `rhessys/`, add it to the
  makefile, and make sure it builds and passes `Testing/regression/regress.sh check`.
- **Otherwise:** delete this folder once no one has asked for any of these files.
