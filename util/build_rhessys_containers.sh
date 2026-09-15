#!/usr/bin/env bash
set -euo pipefail

# Build RHESSys Docker images and convert them to Singularity/Apptainer SIF files.
#
# Core-only flow (baseline):
#   docker build --no-cache -f Dockerfile -t rhessys-local:core .
#   singularity build rhessys-core.sif docker-daemon://rhessys-local:core
#
# FIRE flow differences:
#   docker build --no-cache -f FIRE/Dockerfile -t rhessys-local:fire .
#   singularity build rhessys-fire.sif docker-daemon://rhessys-local:fire

MODE="${1:-core}"
NO_CACHE="${NO_CACHE:-1}"
ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

if [[ "$NO_CACHE" == "1" ]]; then
  CACHE_ARG="--no-cache"
else
  CACHE_ARG=""
fi

require_cmd() {
  if ! command -v "$1" >/dev/null 2>&1; then
    echo "Missing required command: $1" >&2
    exit 1
  fi
}

build_core() {
  echo "[core] Building Docker image rhessys-local:core"
  docker build $CACHE_ARG -f "$ROOT_DIR/"Dockerfile -t rhessys-local:core "$ROOT_DIR"

  echo "[core] Building SIF rhessys-core.sif"
  singularity build "$ROOT_DIR/rhessys-core.sif" docker-daemon://rhessys-local:core

  echo "[core] Done"
}

build_fire() {
  echo "[fire] Building Docker image rhessys-local:fire"
  docker build $CACHE_ARG -f "$ROOT_DIR/FIRE/Dockerfile" -t rhessys-local:fire "$ROOT_DIR"

  echo "[fire] Building SIF rhessys-fire.sif"
  singularity build "$ROOT_DIR/rhessys-fire.sif" docker-daemon://rhessys-local:fire

  echo "[fire] Done"
}

print_usage() {
  cat <<'EOF'
Usage:
  util/build_rhessys_containers.sh [core|fire|all]

Modes:
  core   Build baseline Docker image + rhessys-core.sif (default)
  fire   Build FIRE Docker image + rhessys-fire.sif
  all    Build both core and fire

Options via env vars:
  NO_CACHE=1  Use --no-cache on docker build (default)
  NO_CACHE=0  Allow docker layer cache reuse
EOF
}

main() {
  require_cmd docker
  require_cmd singularity

  case "$MODE" in
    core)
      build_core
      ;;
    fire)
      build_fire
      ;;
    all)
      build_core
      build_fire
      ;;
    -h|--help|help)
      print_usage
      ;;
    *)
      echo "Unknown mode: $MODE" >&2
      print_usage
      exit 1
      ;;
  esac
}

main
