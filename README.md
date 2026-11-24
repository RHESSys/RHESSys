```diff
+ TRUNK IS NOW THE DEFAULT BRANCH (REPLACING MASTER).
+ THE MASTER BRANCH WILL REMAIN FOR ARCHIVAL PURPOSES, BUT TRUNK SHOULD NOW BE USED AS THE MAIN BRANCH.
- SEE NEW CHANGES.    
- NEW CODE MAY NOT BE BACKWARD COMPATIBLE WITH YOUR CURRENT FILES.    
- PLEASE REVIEW INFORMATION ABOUT CHANGES ON THE WHAT'S NEW WIKI PAGE   
```
https://github.com/RHESSys/RHESSys/wiki/What's-New

RHESSys - The Regional Hydro-Ecologic Simulation System
=======================================================

Github is the new home for the RHESSys code repository.

The project homepage is at http://fiesta.bren.ucsb.edu/~rhessys/

The old SVN repository was at http://sourceforge.net/projects/rhessys/ 

Branches
--------
The "develop" branch should be used for day-to-day development, with
RHESSys releases pushed to the "trunk" (formerly "master") branch periodically (for example
yearly).


Continuous Build and Test
-------------------------

Continuous integration runs on GitHub Actions (`.github/workflows/tests.yml`) for pull requests and pushes to `develop`:
unit tests (`make skiptests=F`), a determinism and water-balance check on the W8 regression case, and an
AddressSanitizer/UndefinedBehaviorSanitizer run. See `docs/TESTING.md` for all the tests, including the before/after
regression comparison to run on a code change (`Testing/regression/regress.sh baseline` then `check`).

Travis CI is no longer used. Its configuration is kept as `.travis.yml.disabled` (running the same checks via
`travis.sh`); see that file for how to re-enable it.

While the code is successfully compiling and running, there are a significant number of compiler warnings at this time:

    $ GISBASE=/usr/lib/grass64 make 2>&1 | grep warning | wc -l
    1233

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

    $ wc -l err.txt
    16

Static analysis will show things like memory leaks, out-of-bound references, and null pointers.  It is generally assumed a good thing to have your code be "static analysis clean".
