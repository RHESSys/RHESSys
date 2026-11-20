# make_ward_best_inputs.R
#
# Snapshot the inputs of a calibrated Ward run into a self-contained directory for the
# `ward_long` regression case, so the case does not change when the Ward project does.
#
# The per-ParSetID def copies in the Ward project (defs/<def>/<def>_<i>.def) are overwritten
# by every new calibration batch, so the defs are rebuilt here: the base defs are copied into the
# snapshot, then the run's archived parameter values (robj/cal_archive.rds) are applied with
# RHESSysIOinR::change_def_file() (the same function the calibration uses). NA values (parameters
# not used in that batch) are skipped. Nothing in the Ward project is written.
#
# Usage (any working dir):
#   Rscript make_ward_best_inputs.R [global_id] [ward_dir] [out_dir]
# Defaults: RunID296_345 (best composite score as of 2026-11-19),
#           C:/Users/burke/Documents/Projects/CARB/Ward,
#           C:/Users/burke/Documents/Repos/RHESSys-regression-inputs/ward_best

library(RHESSysIOinR)

args = commandArgs(trailingOnly = TRUE)
run_gid = if (length(args) >= 1) args[1] else "RunID296_345"
ward = if (length(args) >= 2) args[2] else "C:/Users/burke/Documents/Projects/CARB/Ward"
out = if (length(args) >= 3) args[3] else "C:/Users/burke/Documents/Repos/RHESSys-regression-inputs/ward_best"

arch = readRDS(file.path(ward, "robj/cal_archive.rds"))
row = as.data.frame(arch[arch$global_id == run_gid, ])
stopifnot(nrow(row) == 1, row$status == "ok")

if (dir.exists(out)) stop("output dir exists, remove it first: ", out)
dir.create(out, recursive = TRUE)

# ---- hdr layout (from R/0_inputs.R + R/3.1_CALIBRATION_setup.R) ----
hdr = list(
  basin = "defs/basin.def",
  hillslope = "defs/hill.def",
  zone = "defs/zone.def",
  soil = c("defs/soil_sandy-loam.def", "defs/soil_loam.def"),
  landuse = "defs/lu_Ward.def",
  stratum = c("defs/stratum_evergreen.def", "defs/veg_p301_shrub.def", "defs/stratum_grass.def", "defs/stratum_nonveg.def"),
  base_stations = row$batch_base_stations
)

copy_in = function(rel) {
  dst = file.path(out, rel)
  dir.create(dirname(dst), recursive = TRUE, showWarnings = FALSE)
  stopifnot(file.copy(file.path(ward, rel), dst))
}

# ---- base inputs ----
for (f in unlist(hdr[names(hdr) != "base_stations"])) copy_in(f)
copy_in(row$batch_world_file)
copy_in(row$batch_flow_file)
copy_in(row$batch_base_stations)
clim_prefix = sub("\\.base$", "", row$batch_base_stations)
for (ext in c("rain", "tmax", "tmin")) copy_in(paste0(clim_prefix, ".", ext))

# ---- apply archived parameter values ----
par_cols = grep("::", names(row), value = TRUE)
pars = data.frame(
  def = sub("::.*$", "", par_cols),
  name = sub("^.*::", "", par_cols),
  value = vapply(par_cols, function(c) as.character(row[[c]]), ""),
  stringsAsFactors = FALSE
)
pars = pars[!is.na(pars$value) & pars$value != "NA", ]
new_def = list()
for (d in unique(pars$def)) {
  p = pars[pars$def == d, ]
  ps = as.data.frame(t(p$value), stringsAsFactors = FALSE)
  names(ps) = p$name
  new_def[[d]] = change_def_file(file.path(out, d), ps, file_name_ext = "best")
}
rel_def = function(d) if (!is.null(new_def[[d]])) sub(paste0("^", out, "/"), "", new_def[[d]]) else d

# ---- hdr ----
line = function(f, tag) paste0(f, "\t\t", tag)
h = c()
for (lvl in c("basin", "hillslope", "zone", "soil", "landuse", "stratum")) {
  fs = vapply(hdr[[lvl]], rel_def, "")
  h = c(h, paste0(length(fs), "\t\tnum_", lvl, "_default_files"), line(fs, paste0(lvl, "_default_file")))
}
h = c(h, "1\t\tnum_base_stations_files", line(hdr$base_stations, "base_stations_file"))
dir.create(file.path(out, "worldfiles"), showWarnings = FALSE)
writeLines(h, file.path(out, "worldfiles/ward_best.hdr"))

# ---- provenance ----
clim_lines = length(readLines(file.path(out, paste0(clim_prefix, ".rain")))) - 1
info = c(
  paste("global_id:", run_gid),
  paste("composite:", row$composite, " NSE:", row$NSE, " KGE:", row$KGE, " PBIAS:", row$PBIAS),
  paste("source_project:", ward),
  paste("world:", row$batch_world_file), paste("flow:", row$batch_flow_file),
  paste("base_stations:", row$batch_base_stations),
  paste("calibration_dates:", row$batch_start_date, "->", row$batch_end_date),
  paste("calibration_command_options:", row$batch_command_options),
  paste("clim_days:", clim_lines),
  paste("created:", format(Sys.time(), "%Y-%m-%d %H:%M:%S")),
  "parameters applied:",
  paste0("  ", pars$def, "::", pars$name, " = ", pars$value)
)
writeLines(info, file.path(out, "SNAPSHOT_INFO.txt"))
cat(info, sep = "\n")
