RHESSys - The Regional Hydro-Ecologic Simulation System
=======================================================

Github is the new home for the RHESSys code repository.

The project homepage is at http://fiesta.bren.ucsb.edu/~rhessys/

The old SVN repository was at http://sourceforge.net/projects/rhessys/ 

Continuous Build and Test
-------------------------

We are using Travis-CI (http://travis-ci.org) to host our continuous integration efforts.  Continuous integration helps us run our test suite upon every commit to this repository and let us know if and when we break the build.

The current build status is: [![Build Status](https://travis-ci.org/RHESSys/RHESSys.png?branch=master)](https://travis-ci.org/RHESSys/RHESSys)

The above icon should be clickable and point to the latest build at Travis-CI: https://travis-ci.org/RHESSys/RHESSys

The `.travis.yml` configuration file defines how this project is hooked to Travis-CI.  Github has a post-commit hook that is fired upon every commit to this repository.  This post-commit hook uses an authentication token to login to Travis-CI and run the configured steps on a virtual machine.  A return value of 0 means success and generates a 'green' status indicator (hopefully illustrated in the previous paragraph).

Tests
-----

The Create Flowpaths subproject has a growing suite of tests that can be run via `make test`.  Tests are defined as .c files in the `cf/test/src` directory and will automatically get compiled and run by the `make test` target.

Code Coverage
-------------

The Create Flowpaths subproject also has a code coverage script.  This script will use [gcov](http://gcc.gnu.org/onlinedocs/gcc/Gcov.html) and [lcov](http://ltp.sourceforge.net/coverage/lcov.php) to generate an HTML coverage report and show where more tests are needed by illustrating which lines of code are not being exercised by the existing tests.

Run the code coverage script:

    cd cf/
    ./generate_coverage.sh

This will generate an HTML report in the newly formed `cf/coverage_report/` directory.  This new directory can be copied to a webserver or opened directly in your web browser.

Static Analysis
---------------

RHESSys can be analyzed by [cppcheck](http://cppcheck.sourceforge.net/) in a few seconds with the following command:

    cppcheck . --quiet

OR to see the output, and save the errors out to a textfile:

    cppcheck . 2> err.txt
    cat err.txt
    wc -l err.txt

Static analysis will show things like memory leaks, out-of-bound references, and null pointers.  It is generally assumed a good thing to have your code be "static analysis clean".

As of this commit, RHESSys shows 16 errors.
