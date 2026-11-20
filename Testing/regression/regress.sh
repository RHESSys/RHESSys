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
#   RHESSYS_REGRESS_INPUTS  snapshots of external inputs   (default: <repo>/../RHESSys-regression-inputs)
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
# Snapshots of large external inputs (e.g. Ward); case files reference it as $INPUTS
INPUTS="${RHESSYS_REGRESS_INPUTS:-$(cd "$REPO/.." && pwd)/RHESSys-regression-inputs}"
NPROC="$(nproc 2>/dev/null || echo 2)"
# llvm coverage tools must match the clang that built the binary
LLVM_BIN="${LLVM_BIN:-$(dirname "$(readlink -f "$(command -v clang)")")}"

die() { echo "ERROR: $*" >&2; exit 1; }

# make arguments per build config, one per line (so CMD_OPTS can contain spaces)
config_flags() {
  case "$1" in
    default)  printf '%s\n' "skiptests=F" ;;
    fire)     printf '%s\n' "skiptests=F" "wmfire=T" ;;
    # AddressSanitizer + UndefinedBehaviorSanitizer, no LTO; findings go to rhessys.log (halt_on_error=0)
    asan)     printf '%s\n' "skiptests=T" "LTO_FLAGS=" "CMD_OPTS=-O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined" ;;
    asan_fire) printf '%s\n' "skiptests=T" "wmfire=T" "LTO_FLAGS=" "CMD_OPTS=-O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined" ;;
    # clang source-based coverage; profiles written per run to LLVM_PROFILE_FILE
    coverage) printf '%s\n' "skiptests=T" "LTO_FLAGS=" "CMD_OPTS=-O0 -g -fprofile-instr-generate -fcoverage-mapping" ;;
    coverage_fire) printf '%s\n' "skiptests=T" "wmfire=T" "LTO_FLAGS=" "CMD_OPTS=-O0 -g -fprofile-instr-generate -fcoverage-mapping" ;;
    *) die "unknown build config '$1' (known: default fire asan asan_fire coverage coverage_fire)" ;;
  esac
}

load_case() {
  local f="$HERE/cases/$1.case"
  [ -f "$f" ] || die "no case file $f"
  unset CASE_DESC CASE_BUILD CASE_WORLD CASE_HDR CASE_FLOW CASE_TEC CASE_FILTER CASE_START CASE_END CASE_FLAGS CASE_ROOT CASE_LINKS
  # shellcheck disable=SC1090
  source "$f"
  # BUILD=<config> runs the case with another build (e.g. asan, coverage)
  CASE_BUILD="${BUILD:-$CASE_BUILD}"
  CASE_ROOT="${CASE_ROOT:-$TESTING}"
  CASE_LINKS="${CASE_LINKS:-defs clim flowtables fire_grid}"
}

do_build() {
  local cfg="$1" flags out
  local -a margs
  mapfile -t margs < <(config_flags "$cfg")
  flags="${margs[*]}"
  out="$WORK/bin/$cfg"
  mkdir -p "$out" "$WORK/bin/lib"
  echo "== build $cfg ($flags)"
  ( cd "$REPO/rhessys" && make clean >/dev/null 2>&1 || true
    make -j"$NPROC" "${margs[@]}" > "$out/build.log" 2>&1 ) || { tail -30 "$out/build.log"; die "build $cfg failed (log: $out/build.log)"; }
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
  ( cd "$1" && find . -type f ! -path './inputs/*' ! -path './prof/*' ! -name 'rhessys.log' ! -name 'time.log' ! -name 'cmd.txt' \
      ! -name 'run_info.txt' ! -name 'MANIFEST.sha256' | sed 's|^\./||' | LC_ALL=C sort )
}

do_run() {
  local case="$1" dest="${2:-$WORK/runs/$1}"
  load_case "$case"
  local bin="$WORK/bin/$CASE_BUILD/rhessys"
  [ -x "$bin" ] || die "binary for config '$CASE_BUILD' not built; run: $0 build $CASE_BUILD"
  safe_rm_dir "$dest"
  mkdir -p "$dest/out" "$dest/inputs"
  # Input dirs are symlinked from the case's input root. The worldfile is symlinked into a real
  # worldfiles/ dir so the .state output (named after the -w path) lands in the run dir.
  # hdr is relative to CASE_ROOT; tec and filter are relative to Testing/ (kept in this repo).
  local root="$CASE_ROOT"
  [ -d "$root" ] || die "input root $root for case $case does not exist"
  for d in $CASE_LINKS; do [ -e "$root/$d" ] && ln -s "$root/$d" "$dest/$d"; done
  mkdir -p "$dest/worldfiles"; ln -s "$root/$CASE_WORLD" "$dest/$CASE_WORLD"
  mkdir -p "$dest/inputs/$(dirname "$CASE_HDR")"; cp "$root/$CASE_HDR" "$dest/inputs/$CASE_HDR"
  for f in "$CASE_TEC" "$CASE_FILTER"; do
    mkdir -p "$dest/inputs/$(dirname "$f")"; cp "$TESTING/$f" "$dest/inputs/$f"
  done
  ( cd "$root" && sha256sum "$CASE_WORLD" "$CASE_FLOW" "$CASE_HDR" ) > "$dest/inputs/input_checksums.sha256"
  local cmd="$bin -t inputs/$CASE_TEC -w $CASE_WORLD -whdr inputs/$CASE_HDR -r $CASE_FLOW -st $CASE_START -ed $CASE_END -of inputs/$CASE_FILTER $CASE_FLAGS"
  echo "$cmd" > "$dest/cmd.txt"
  echo "== run $case -> $dest"
  local rc=0
  # shellcheck disable=SC2086
  ( cd "$dest" && LLVM_PROFILE_FILE="$dest/prof/%p.profraw" ASAN_OPTIONS="${ASAN_OPTIONS:-detect_leaks=1}" \
      UBSAN_OPTIONS="${UBSAN_OPTIONS:-print_stacktrace=1}" /usr/bin/time -v -o time.log $cmd > rhessys.log 2>&1 ) || rc=$?
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
  local rc=0
  do_compare_dirs "$GOLDEN/$case" "$WORK/runs/$case" || rc=1
  python3 "$HERE/invariants.py" "$WORK/runs/$case" || rc=1
  return $rc
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
  sanitize)
    # run cases with the ASan/UBSan build (asan, or asan_fire for fire cases); summarize reports
    rc=0
    for c in "$@"; do
      load_case "$c"; cfg=asan; [ "$CASE_BUILD" = fire ] && cfg=asan_fire
      [ -x "$WORK/bin/$cfg/rhessys" ] || do_build "$cfg"
      ( BUILD=$cfg do_run "$c" "$WORK/runs/$c.$cfg" ) || true
      log="$WORK/runs/$c.$cfg/rhessys.log"
      echo "== sanitizer summary $c ($cfg): $log"
      grep -E "runtime error:|ERROR: AddressSanitizer|ERROR: LeakSanitizer|SUMMARY:" "$log" | sed 's/^/   /' | sort | uniq -c | sort -rn | head -40
      # memory errors and UB fail; leaks (mostly structures never freed at exit) are reported only
      grep -qE "runtime error:|ERROR: AddressSanitizer" "$log" && rc=1
      grep -q "ERROR: LeakSanitizer" "$log" && echo "   (leaks reported above are not counted as failures)"
    done; exit $rc ;;
  coverage)
    # run cases with the coverage build(s), merge profiles, write a per-file report to $WORK/coverage
    out="$WORK/coverage"; mkdir -p "$out"; profs=(); bins=()
    for c in "$@"; do
      load_case "$c"; cfg=coverage; [ "$CASE_BUILD" = fire ] && cfg=coverage_fire
      [ -x "$WORK/bin/$cfg/rhessys" ] || do_build "$cfg"
      BUILD=$cfg do_run "$c" "$WORK/runs/$c.$cfg"
      profs+=("$WORK/runs/$c.$cfg"/prof/*.profraw); bins+=("$WORK/bin/$cfg/rhessys")
    done
    "$LLVM_BIN/llvm-profdata" merge -o "$out/merged.profdata" "${profs[@]}"
    objs=(); for b in $(printf '%s\n' "${bins[@]}" | sort -u); do objs+=(-object "$b"); done
    "$LLVM_BIN/llvm-cov" report "${objs[@]:1}" -instr-profile="$out/merged.profdata" > "$out/report.txt" 2>"$out/report.err" || true
    "$LLVM_BIN/llvm-cov" export -format=text -summary-only "${objs[@]:1}" -instr-profile="$out/merged.profdata" > "$out/summary.json" 2>/dev/null || true
    echo "== coverage report: $out/report.txt (cases: $*)"; tail -1 "$out/report.txt" ;;
  list) for f in "$HERE"/cases/*.case; do n="$(basename "$f" .case)"; load_case "$n"; printf '%-14s %s\n' "$n" "$CASE_DESC"; done ;;
  *) sed -n '2,22p' "$0"; exit 1 ;;
esac
