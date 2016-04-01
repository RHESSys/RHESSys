#!/bin/sh
# Build binaries for all RHESSys sub-components
make
# Run unit tests for CF
cd cf ; make test ; cd -
# Run unit tests for RHESSys
cd rhessys && make test -
# Run functional tests for RHESSys
make functest -
