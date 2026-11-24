#!/bin/sh
# Shared CI test script: called by the (disabled) Travis config .travis.yml.disabled;
# mirrors the GitHub Actions workflow .github/workflows/tests.yml. Also fine to run locally.
# Build RHESSys with unit tests (a failing unit test fails the build), then run the
# fast regression case's determinism and water-balance check (no reference outputs needed).
set -e
cd rhessys && make clean && make skiptests=F && cd -
Testing/regression/regress.sh build default
Testing/regression/regress.sh determinism w8_fast
