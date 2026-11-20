#!/usr/bin/env bash
# RHESSys regression harness.
#
# Usage:
#   regress.sh build <config>...        build rhessys for a config (default | fire) into $WORK/bin/<config>
#   regress.sh run <case>...            run a case (cases/<case>.case) into $WORK/runs/<case>
#   regress.sh compare <case>...        compare $WORK/runs/<case> against $GOLDEN/<case>
#   regress.sh check <case>...          build (if needed) + run + compare
#   regress.sh determinism <case>...    run a case twice and compare the two runs
#   regress.sh bless <case>...          copy $WORK/runs/<case> to $GOLDEN/<case> (refuses to overwrite unless FORCE=1)
#   regress.sh list                     list cases
#
# Environment:
#   RHESSYS_REGRESS_WORK  scratch dir for builds and runs  (default: $HOME/rhessys_regress)
#   RHESSYS_GOLDEN        golden output dir                (default: <repo>/../RHESSys-regression-golden)
#   REBUILD=1             force rebuild in `check`
#   FORCE=1               allow `bless` to overwrite an existing golden case
#   TOL_ABS / TOL_REL     tolerances passed to compare.py (default 0 = bitwise)
#
# Notes:
#   - `build` runs `make clean` in rhessys/, so it discards the objects in your working build.
#   - The default config builds with skiptests=F, so unit tests run (and must pass) during the build.
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TESTING="$(cd "$HERE/.." && pwd)"
REPO="$(cd "$TESTING/.." && pwd)"
WORK="${RHESSYS_REGRESS_WORK:-$HOME/rhessys_regress}"
GOLDEN="${RHESSYS_GOLDEN:-$(cd "$REPO/.." && pwd)/RHESSys-regression-golden}"
NPROC="$(nproc 2>/dev/null || echo 2)"

die() { echo "ERROR: $*" >&2; exit 1; }

config_flags() {
  case "$1" in
    default) echo "skiptests=F" ;;
    fire)    echo "skiptests=F wmfire=T" ;;
    *) die "unknown build config '$1' (known: default fire)" ;;
  esac
}

load_case() {
  local f="$HERE/cases/$1.case"
  [ -f "$f" ] || die "no case file $f"
  unset CASE_DESC CASE_BUILD CASE_WORLD CASE_HDR CASE_FLOW CASE_TEC CASE_FILTER CASE_START CASE_END CASE_FLAGS
  # shellcheck disable=SC1090
  source "$f"
}

do_build() {
  local cfg="$1" flags out
  flags="$(config_flags "$cfg")"
  out="$WORK/bin/$cfg"
  mkdir -p "$out" "$WORK/bin/lib"
  echo "== build $cfg ($flags)"
  ( cd "$REPO/rhessys" && make clean >/dev/null 2>&1 || true
    # shellcheck disable=SC2086
    make -j"$NPROC" $flags > "$out/build.log" 2>&1 ) || { tail -30 "$out/build.log"; die "build $cfg failed (log: $out/build.log)"; }
  local pgm
  pgm="$(cd "$REPO/rhessys" && make -s print-PGM | sed 's/^PGM = //')"
  cp "$REPO/rhessys/$pgm" "$out/rhessys"
  cp "$REPO/lib/libwmfire.so" "$WORK/bin/lib/libwmfire.so" 2>/dev/null || true
  {
    echo "config: $cfg"
    echo "make_flags: $flags"
    echo "git_commit: $(git -C "$REPO" rev-parse HEAD)"
    echo "git_branch: $(git -C "$REPO" rev-parse --abbrev-ref HEAD)"
    echo "git_dirty_rhessys_src: $(git -C "$REPO" status --porcelain -- rhessys | grep -v '^??' | wc -l) modified tracked files"
    echo "git_diff_sha256: $(git -C "$REPO" diff HEAD -- rhessys | sha256sum | cut -d' ' -f1)"
    echo "binary_sha256: $(sha256sum "$out/rhessys" | cut -d' ' -f1)"
    echo "libwmfire_sha256: $(sha256sum "$REPO/lib/libwmfire.so" 2>/dev/null | cut -d' ' -f1)"
    echo "cc: $(cd "$REPO/rhessys" && make -s print-CC | sed 's/^CC = //') / $(clang --version 2>/dev/null | head -1)"
    echo "unit_tests: $(grep -c '^ok ' "$out/build.log" || true) ok, $(grep -c '^not ok' "$out/build.log" || true) not ok"
    echo "built: $(date -Iseconds)"
  } > "$out/build_info.txt"
  cat "$out/build_info.txt"
}

safe_rm_dir() {  # only removes dirs under $WORK
  local d="$1"
  case "$d" in "$WORK"/runs/*) [ -d "$d" ] && rm -r -- "$d" ;; *) die "refusing to remove $d" ;; esac
  return 0
}

# Files in a run dir that are outputs (everything except inputs, symlinks and logs)
list_outputs() {
  ( cd "$1" && find . -type f ! -path './inputs/*' ! -name 'rhessys.log' ! -name 'time.log' ! -name 'cmd.txt' \
      ! -name 'run_info.txt' ! -name 'MANIFEST.sha256' | sed 's|^\./||' | LC_ALL=C sort )
}

do_run() {
  local case="$1" dest="${2:-$WORK/runs/$1}"
  load_case "$case"
  local bin="$WORK/bin/$CASE_BUILD/rhessys"
  [ -x "$bin" ] || die "binary for config '$CASE_BUILD' not built; run: $0 build $CASE_BUILD"
  safe_rm_dir "$dest"
  mkdir -p "$dest/out" "$dest/inputs"
  # read-only inputs are symlinked; files that rhessys writes next to (worldfile -> .state) are copied
  for d in defs clim flowtables fire_grid; do [ -e "$TESTING/$d" ] && ln -s "$TESTING/$d" "$dest/$d"; done
  for f in "$CASE_WORLD" "$CASE_HDR" "$CASE_TEC" "$CASE_FILTER"; do
    mkdir -p "$dest/inputs/$(dirname "$f")"; cp "$TESTING/$f" "$dest/inputs/$f"
  done
  mkdir -p "$dest/worldfiles"; cp "$TESTING/$CASE_WORLD" "$dest/$CASE_WORLD"
  local cmd="$bin -t inputs/$CASE_TEC -w $CASE_WORLD -whdr inputs/$CASE_HDR -r $CASE_FLOW -st $CASE_START -ed $CASE_END -of inputs/$CASE_FILTER $CASE_FLAGS"
  echo "$cmd" > "$dest/cmd.txt"
  echo "== run $case -> $dest"
  local rc=0
  # shellcheck disable=SC2086
  ( cd "$dest" && /usr/bin/time -v -o time.log $cmd > rhessys.log 2>&1 ) || rc=$?
  # the worldfile copy next to the .state output is an input (also saved under inputs/), not an output
  rm -f -- "$dest/$CASE_WORLD"
  local wall rss
  wall="$(grep 'Elapsed (wall clock)' "$dest/time.log" | awk '{print $NF}')"
  rss="$(grep 'Maximum resident set size' "$dest/time.log" | awk '{print $NF}')"
  {
    echo "case: $case"; echo "desc: $CASE_DESC"; echo "exit_code: $rc"
    echo "wall_clock: $wall"; echo "max_rss_kb: $rss"; echo "run: $(date -Iseconds)"
    cat "$WORK/bin/$CASE_BUILD/build_info.txt"
  } > "$dest/run_info.txt"
  ( cd "$dest" && list_outputs . | xargs -r -d '\n' sha256sum > MANIFEST.sha256 )
  echo "   exit=$rc wall=$wall max_rss_kb=$rss outputs=$(wc -l < "$dest/MANIFEST.sha256")"
  [ "$rc" -eq 0 ] || { tail -20 "$dest/rhessys.log"; die "rhessys exited $rc for case $case"; }
}

do_compare_dirs() {  # <reference dir> <test dir>
  python3 "$HERE/compare.py" --tol-abs "${TOL_ABS:-0}" --tol-rel "${TOL_REL:-0}" "$1" "$2"
}

do_compare() {
  local case="$1"
  [ -d "$GOLDEN/$case" ] || die "no golden output for $case in $GOLDEN"
  [ -d "$WORK/runs/$case" ] || die "no run for $case; run: $0 run $case"
  echo "== compare $case: golden=$GOLDEN/$case run=$WORK/runs/$case"
  do_compare_dirs "$GOLDEN/$case" "$WORK/runs/$case"
}

do_bless() {
  local case="$1" src="$WORK/runs/$1" dst="$GOLDEN/$1"
  [ -f "$src/MANIFEST.sha256" ] || die "no completed run for $case"
  if [ -e "$dst" ]; then
    [ "${FORCE:-0}" = 1 ] || die "$dst exists; set FORCE=1 to overwrite"
    mv "$dst" "$dst.replaced.$(date +%Y%m%d%H%M%S)"
  fi
  mkdir -p "$dst"
  ( cd "$src" && { list_outputs .; echo MANIFEST.sha256; echo run_info.txt; echo cmd.txt; echo rhessys.log; echo time.log; } \
      | while read -r f; do mkdir -p "$dst/$(dirname "$f")"; cp "$f" "$dst/$f"; done )
  cp -r "$src/inputs" "$dst/inputs"
  echo "== blessed $case -> $dst"
}

cmd="${1:-}"; shift || true
case "$cmd" in
  build) [ $# -gt 0 ] || set -- default fire; for c in "$@"; do do_build "$c"; done ;;
  run) for c in "$@"; do do_run "$c"; done ;;
  compare) rc=0; for c in "$@"; do do_compare "$c" || rc=1; done; exit $rc ;;
  check)
    rc=0
    for c in "$@"; do
      load_case "$c"
      if [ "${REBUILD:-0}" = 1 ] || [ ! -x "$WORK/bin/$CASE_BUILD/rhessys" ]; then do_build "$CASE_BUILD"; fi
      do_run "$c"; do_compare "$c" || rc=1
    done; exit $rc ;;
  determinism)
    rc=0
    for c in "$@"; do
      do_run "$c"; do_run "$c" "$WORK/runs/$c.rep2"
      echo "== determinism $c"; do_compare_dirs "$WORK/runs/$c" "$WORK/runs/$c.rep2" || rc=1
    done; exit $rc ;;
  bless) for c in "$@"; do do_bless "$c"; done ;;
  list) for f in "$HERE"/cases/*.case; do n="$(basename "$f" .case)"; load_case "$n"; printf '%-14s %s\n' "$n" "$CASE_DESC"; done ;;
  *) sed -n '2,22p' "$0"; exit 1 ;;
esac
