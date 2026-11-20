#!/bin/sh
# Build RHESSys with unit tests (a failing unit test fails the build), then run the
# fast regression case's determinism check (no golden outputs needed).
# Golden-output comparisons need the local golden dir: see docs/TESTING.md.
set -e
cd rhessys && make clean && make skiptests=F && cd -
Testing/regression/regress.sh build default
Testing/regression/regress.sh determinism w8_fast
python3 Testing/regression/invariants.py "${RHESSYS_REGRESS_WORK:-$HOME/rhessys_regress}/runs/w8_fast"
