#!/usr/bin/env bash
# RHESSys regression harness. See docs/TESTING.md for the workflow.
#
# Usage:
#   regress.sh baseline <case>...       build the base commit (BASE_REF, default HEAD) in a temporary git
#                                       worktree, run it, and save the outputs as the reference for <case>
#   regress.sh check <case>...          rebuild your working tree, run, compare to the reference
#   regress.sh build <config>...        build rhessys for a config (default | fire) into $WORK/bin/<config>
#   regress.sh run <case>...            run a case (cases/<case>.case) into $WORK/runs/<case>
#   regress.sh compare <case>...        compare $WORK/runs/<case> against the reference $GOLDEN/<case>
#   regress.sh bless <case>...          save $WORK/runs/<case> as the reference (FORCE=1 to replace a blessed one)
#   regress.sh determinism <case>...    run a case twice and compare the two runs (no reference needed)
#   regress.sh sanitize <case>...       run with the ASan/UBSan build, summarize findings
#   regress.sh list                     list cases
#
# Environment:
#   RHESSYS_REGRESS_WORK  scratch dir for builds, runs and references (default: $HOME/rhessys_regress)
#   RHESSYS_GOLDEN        reference output dir                        (default: $WORK/golden)
#   BASE_REF              git commit/branch/tag that `baseline` builds  (default: HEAD)
#   REBUILD=0             `check` reuses an existing build instead of rebuilding
#   FORCE=1               allow replacing a blessed reference (the old copy is kept as .replaced.<time>)
#   TOL_ABS / TOL_REL     tolerances passed to compare.py (default 0 = bitwise)
#   RUN_WRAP              command prefix for the model run (e.g. "valgrind --tool=memcheck")
#   RHESSYS_REGRESS_EXT   optional dir with extra cases (cases/*.case) and commands/configs (regress_ext.sh)
#
# Notes:
#   - `build` runs `make clean` in rhessys/, so it discards the objects in your working build.
#   - The default config builds with skiptests=F, so unit tests run (and must pass) during the build.
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TESTING="$(cd "$HERE/.." && pwd)"
REPO="$(cd "$TESTING/.." && pwd)"
WORK="${RHESSYS_REGRESS_WORK:-$HOME/rhessys_regress}"
GOLDEN="${RHESSYS_GOLDEN:-$WORK/golden}"
EXT="${RHESSYS_REGRESS_EXT:-}"
NPROC="$(nproc 2>/dev/null || echo 2)"

die() { echo "ERROR: $*" >&2; exit 1; }

# make arguments per build config, one per line (so CMD_OPTS can contain spaces)
config_flags() {
  case "$1" in
    default)  printf '%s\n' "skiptests=F" ;;
    fire)     printf '%s\n' "skiptests=F" "wmfire=T" ;;
    # AddressSanitizer + UndefinedBehaviorSanitizer, no LTO; findings go to rhessys.log (halt_on_error=0)
    asan)     printf '%s\n' "skiptests=T" "LTO_FLAGS=" "CMD_OPTS=-O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined" ;;
    asan_fire) printf '%s\n' "skiptests=T" "wmfire=T" "LTO_FLAGS=" "CMD_OPTS=-O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined" ;;
    *) if declare -F ext_config_flags >/dev/null && ext_config_flags "$1"; then return 0; fi
       die "unknown build config '$1' (known: default fire asan asan_fire)" ;;
  esac
}

# prints the case file for <case>: this repo's cases/ first, then $EXT/cases
case_file() {
  local d
  for d in "$HERE/cases" ${EXT:+"$EXT/cases"}; do
    [ -f "$d/$1.case" ] && { echo "$d/$1.case"; return 0; }
  done
  die "no case '$1' (see: $0 list)"
}

load_case() {
  local f
  f="$(case_file "$1")"
  unset CASE_DESC CASE_BUILD CASE_WORLD CASE_HDR CASE_FLOW CASE_TEC CASE_FILTER CASE_START CASE_END CASE_FLAGS CASE_ROOT CASE_LINKS CASE_INPUTS
  # shellcheck disable=SC1090
  source "$f"
  # BUILD=<config> runs the case with another build (e.g. asan)
  CASE_BUILD="${BUILD:-$CASE_BUILD}"
  # CASE_ROOT: model inputs (worldfile, hdr, flow, defs, clim). CASE_INPUTS: tec and filter files.
  # Both default to Testing/ for this repo's cases; extension cases default CASE_INPUTS to $EXT.
  CASE_ROOT="${CASE_ROOT:-$TESTING}"
  if [ "$(dirname "$f")" = "$HERE/cases" ]; then CASE_INPUTS="${CASE_INPUTS:-$TESTING}"; else CASE_INPUTS="${CASE_INPUTS:-$EXT}"; fi
  CASE_LINKS="${CASE_LINKS:-defs clim flowtables fire_grid}"
}

# do_build <config> [source repo (default: this repo)] [output name (default: <config>)] [extra make args...]
do_build() {
  local cfg="$1" src="${2:-$REPO}" name="${3:-$1}" flags out
  shift $(( $# < 3 ? $# : 3 ))
  local -a margs
  mapfile -t margs < <(config_flags "$cfg")
  margs+=("$@")
  flags="${margs[*]}"
  out="$WORK/bin/$name"
  mkdir -p "$out" "$WORK/bin/lib"
  echo "== build $name from $src ($flags)"
  ( cd "$src/rhessys" && make clean >/dev/null 2>&1 || true
    make -j"$NPROC" "${margs[@]}" > "$out/build.log" 2>&1 ) || { tail -30 "$out/build.log"; die "build $name failed (log: $out/build.log)"; }
  local pgm
  pgm="$(cd "$src/rhessys" && make -s print-PGM | sed 's/^PGM = //')"
  cp "$src/rhessys/$pgm" "$out/rhessys"
  cp "$REPO/lib/libwmfire.so" "$WORK/bin/lib/libwmfire.so" 2>/dev/null || true
  {
    echo "config: $cfg"
    echo "make_flags: $flags"
    echo "source: $src"
    echo "git_commit: $(git -C "$src" rev-parse HEAD)"
    echo "git_branch: $(git -C "$src" rev-parse --abbrev-ref HEAD)"
    echo "git_dirty_rhessys_src: $(git -C "$src" status --porcelain -- rhessys | grep -v '^??' | wc -l) modified tracked files"
    echo "git_diff_sha256: $(git -C "$src" diff HEAD -- rhessys | sha256sum | cut -d' ' -f1)"
    echo "binary_sha256: $(sha256sum "$out/rhessys" | cut -d' ' -f1)"
    echo "libwmfire_sha256: $(sha256sum "$REPO/lib/libwmfire.so" 2>/dev/null | cut -d' ' -f1)"
    echo "cc: $(cd "$src/rhessys" && make -s print-CC | sed 's/^CC = //') / $(clang --version 2>/dev/null | head -1)"
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
      ! -name 'run_info.txt' ! -name 'MANIFEST.sha256' ! -name 'BASELINE_INFO.txt' ! -name 'callgrind.out*' | sed 's|^\./||' | LC_ALL=C sort )
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
  # hdr is relative to CASE_ROOT; tec and filter are relative to CASE_INPUTS.
  local root="$CASE_ROOT"
  [ -d "$root" ] || die "input root $root for case $case does not exist"
  for d in $CASE_LINKS; do [ -e "$root/$d" ] && ln -s "$root/$d" "$dest/$d"; done
  mkdir -p "$dest/worldfiles"; ln -s "$root/$CASE_WORLD" "$dest/$CASE_WORLD"
  mkdir -p "$dest/inputs/$(dirname "$CASE_HDR")"; cp "$root/$CASE_HDR" "$dest/inputs/$CASE_HDR"
  for f in "$CASE_TEC" "$CASE_FILTER"; do
    mkdir -p "$dest/inputs/$(dirname "$f")"; cp "$CASE_INPUTS/$f" "$dest/inputs/$f"
  done
  ( cd "$root" && sha256sum "$CASE_WORLD" "$CASE_FLOW" "$CASE_HDR" ) > "$dest/inputs/input_checksums.sha256"
  local cmd="$bin -t inputs/$CASE_TEC -w $CASE_WORLD -whdr inputs/$CASE_HDR -r $CASE_FLOW -st $CASE_START -ed $CASE_END -of inputs/$CASE_FILTER $CASE_FLAGS"
  echo "$cmd" > "$dest/cmd.txt"
  echo "== run $case -> $dest"
  local rc=0
  # shellcheck disable=SC2086
  ( cd "$dest" && LLVM_PROFILE_FILE="$dest/prof/%p.profraw" ASAN_OPTIONS="${ASAN_OPTIONS:-detect_leaks=1}" \
      UBSAN_OPTIONS="${UBSAN_OPTIONS:-print_stacktrace=1}" /usr/bin/time -v -o time.log ${RUN_WRAP:-} $cmd > rhessys.log 2>&1 ) || rc=$?
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
  [ -d "$GOLDEN/$case" ] || die "no reference outputs for $case in $GOLDEN; make them with: $0 baseline $case"
  [ -d "$WORK/runs/$case" ] || die "no run for $case; run: $0 run $case"
  echo "== compare $case: reference=$GOLDEN/$case run=$WORK/runs/$case"
  [ -f "$GOLDEN/$case/BASELINE_INFO.txt" ] && sed 's/^/   /' "$GOLDEN/$case/BASELINE_INFO.txt"
  local rc=0
  do_compare_dirs "$GOLDEN/$case" "$WORK/runs/$case" || rc=1
  python3 "$HERE/invariants.py" "$WORK/runs/$case" || rc=1
  return $rc
}

# store_reference <case> <run dir> <note or empty>: copy a completed run to $GOLDEN/<case>.
# A reference made by `baseline` (has BASELINE_INFO.txt) is replaced freely; a blessed one needs FORCE=1.
store_reference() {
  local case="$1" src="$2" note="$3" dst="$GOLDEN/$1"
  [ -n "$case" ] || die "store_reference: no case"
  [ -f "$src/MANIFEST.sha256" ] || die "no completed run for $case in $src"
  if [ -e "$dst" ]; then
    if [ -f "$dst/BASELINE_INFO.txt" ]; then
      rm -r -- "$dst"
    else
      [ "${FORCE:-0}" = 1 ] || die "$dst is a blessed reference; set FORCE=1 to replace it (the old copy is kept)"
      mv "$dst" "$dst.replaced.$(date +%Y%m%d%H%M%S)"
    fi
  fi
  mkdir -p "$dst"
  ( cd "$src" && { list_outputs .; echo MANIFEST.sha256; echo run_info.txt; echo cmd.txt; echo rhessys.log; echo time.log; } \
      | while read -r f; do mkdir -p "$dst/$(dirname "$f")"; cp "$f" "$dst/$f"; done )
  cp -r "$src/inputs" "$dst/inputs"
  [ -n "$note" ] && printf '%s\n' "$note" > "$dst/BASELINE_INFO.txt"
  echo "== reference for $case -> $dst"
}

# do_baseline <case>...: build BASE_REF in a temporary worktree, run each case, store as the reference.
# The base commit's code is run with this working tree's test inputs (cases, tec, filters, Testing/ data).
do_baseline() {
  local ref="${BASE_REF:-HEAD}" src="$WORK/baseline_src" sha built=" " c
  sha="$(git -C "$REPO" rev-parse --verify --quiet "$ref^{commit}")" || die "unknown git ref '$ref'"
  for c in "$@"; do  # fail early on unknown cases or a blessed reference in the way
    load_case "$c"
    if [ -e "$GOLDEN/$c" ] && [ ! -f "$GOLDEN/$c/BASELINE_INFO.txt" ] && [ "${FORCE:-0}" != 1 ]; then
      die "$GOLDEN/$c is a blessed reference; set FORCE=1 to replace it (the old copy is kept)"
    fi
  done
  git -C "$REPO" worktree remove --force "$src" >/dev/null 2>&1 || true
  [ -e "$src" ] && rm -rf -- "$src"
  git -C "$REPO" worktree prune
  echo "== baseline: $ref = $(git -C "$REPO" log -1 --format='%h %s' "$sha")"
  git -C "$REPO" worktree add --detach --quiet "$src" "$sha"
  # libwmfire.so is not in git; the fire build links the one in this repo's lib/
  [ -f "$REPO/lib/libwmfire.so" ] && { mkdir -p "$src/lib"; cp "$REPO/lib/libwmfire.so" "$src/lib/"; }
  for c in "$@"; do
    load_case "$c"
    local cfg="$CASE_BUILD"
    case "$built" in *" $cfg "*) ;; *) do_build "$cfg" "$src" "baseline_$cfg" skiptests=T; built="$built$cfg " ;; esac
    BUILD="baseline_$cfg" do_run "$c" "$WORK/runs/$c.baseline"
    store_reference "$c" "$WORK/runs/$c.baseline" \
      "baseline: code from $ref ($sha), test inputs from the working tree; made $(date -Iseconds)"
  done
  git -C "$REPO" worktree remove --force "$src"
}

# Optional extension: extra cases, build configs (ext_config_flags) and commands (ext_cmd_<name>)
if [ -n "$EXT" ]; then
  [ -d "$EXT" ] || die "RHESSYS_REGRESS_EXT=$EXT is not a directory"
  # shellcheck disable=SC1091
  [ -f "$EXT/regress_ext.sh" ] && source "$EXT/regress_ext.sh"
fi

cmd="${1:-}"; shift || true
case "$cmd" in
  baseline) [ $# -gt 0 ] || die "usage: [BASE_REF=<ref>] $0 baseline <case>..."; do_baseline "$@" ;;
  build) [ $# -gt 0 ] || set -- default fire; for c in "$@"; do do_build "$c"; done ;;
  run) for c in "$@"; do do_run "$c"; done ;;
  compare) rc=0; for c in "$@"; do do_compare "$c" || rc=1; done; exit $rc ;;
  check)
    # rebuilds the working tree once per build config (REBUILD=0 reuses an existing binary)
    rc=0; built=" "
    for c in "$@"; do
      load_case "$c"
      case "$built" in *" $CASE_BUILD "*) ;; *)
        if [ "${REBUILD:-1}" = 1 ] || [ ! -x "$WORK/bin/$CASE_BUILD/rhessys" ]; then do_build "$CASE_BUILD"; fi
        built="$built$CASE_BUILD " ;;
      esac
      do_run "$c"; do_compare "$c" || rc=1
    done; exit $rc ;;
  determinism)
    rc=0
    for c in "$@"; do
      do_run "$c"; do_run "$c" "$WORK/runs/$c.rep2"
      echo "== determinism $c"; do_compare_dirs "$WORK/runs/$c" "$WORK/runs/$c.rep2" || rc=1
      python3 "$HERE/invariants.py" "$WORK/runs/$c" || rc=1
    done; exit $rc ;;
  bless) for c in "$@"; do store_reference "$c" "$WORK/runs/$c" ""; done ;;
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
  list)
    for f in "$HERE"/cases/*.case ${EXT:+"$EXT"/cases/*.case}; do
      [ -f "$f" ] || continue; n="$(basename "$f" .case)"; load_case "$n"; printf '%-14s %s\n' "$n" "$CASE_DESC"
    done ;;
  *)
    if [ -n "$cmd" ] && declare -F "ext_cmd_$cmd" >/dev/null; then "ext_cmd_$cmd" "$@"; exit $?; fi
    sed -n '2,29p' "$0"; exit 1 ;;
esac
