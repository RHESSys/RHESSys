#!/bin/bash -e

# variables
OUTDIR="coverage_report"
BASEINFOFILE="coverage-base.info"
TESTINFOFILE="coverage-test.info"
TOTALINFOFILE="coverage-total.info"

# honor user specified target directory for coverage results
if [ "$1" != "" ] ; then
    OUTDIR=$1
fi

# get into the correct directory
DETECTEDDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DETECTEDDIR/

# error checking
GCOV=`which gcov`
if [ "$?" -ne "0" ] ; then
    echo "ERROR :: gcov is not in your path" 1>&2
    exit 1
fi
GENINFO=`which lcov`
if [ "$?" -ne "0" ] ; then
    echo "ERROR :: lcov is not in your path" 1>&2
    echo "      :: lcov source: http://downloads.sourceforge.net/ltp/lcov-1.9.tar.gz" 1>&2
    exit 1
fi
GENHTML=`which genhtml`
if [ "$?" -ne "0" ] ; then
    echo "ERROR :: genhtml is not in your path" 1>&2
    echo "      :: lcov source: http://downloads.sourceforge.net/ltp/lcov-1.9.tar.gz" 1>&2
    exit 1
fi

# build project
FOUNDGISBASE=`find /usr -name gis.h | xargs dirname | xargs dirname | xargs dirname`
COVERAGESETTING=1
echo "GISBASE DETECTED [$FOUNDGISBASE]"
echo "COVERAGE SETTING [$COVERAGESETTING]"
echo "-------------- THE MAKE"
make clean
GISBASE=$FOUNDGISBASE COVERAGE=$COVERAGESETTING make

# build coverage baseline
echo "-------------- THE ZEROING"
lcov -f -z --directory . -b .                       # zero counters
echo "-------------- THE BASELINE"
lcov -f -c -i --directory . -b . -o $BASEINFOFILE   # create baseline

# run test suite to exercise the code
echo "-------------- THE TEST RUN"
GISBASE=$FOUNDGISBASE COVERAGE=$COVERAGESETTING make test

# generate the lcov results
echo "-------------- THE GCOV"
for f in $(find . -name "*.c"); do gcov -o `dirname $f | xargs dirname`/objects $f; done
echo "-------------- THE COVERAGE"
lcov -f -c --directory . -b . -o $TESTINFOFILE                      # generate coverage file
echo "-------------- THE COMBINE"
lcov -a $BASEINFOFILE -a $TESTINFOFILE -o $TOTALINFOFILE            # combine base and test
lcov -r $TOTALINFOFILE "/usr/*" --directory . -o $TOTALINFOFILE     # remove /usr/* lines from file
echo "-------------- THE HTML"
rm -rf $OUTDIR                                                      # remove existing report
genhtml -o $OUTDIR $TOTALINFOFILE                                   # generate html report

# clean up
rm -f $BASEINFOFILE
rm -f $TESTINFOFILE
rm -f $TOTALINFOFILE

# print out helpful information
FULLOUTDIR=`readlink -f $OUTDIR`/
echo ""
echo "--------------------------------------"
echo "lcov report generated and saved into:"
echo "  $FULLOUTDIR"
echo ""

