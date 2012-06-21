#!/bin/bash
# This shell script runs the cst (Create Stream Table) program

debug_flag=
verbose_flag=

USAGE="Usage: `basename $0` [-dhv]"

# Parse command line options.
while getopts dhv OPT; do
    case "$OPT" in
        d)
            debug_flag='-d'
            ;;
        h)
            echo $USAGE
            ;;
        v)
            verbose_flag='-v'
            ;;
        \?)
            # getopts issues an error message
            echo $USAGE >&2
            exit 1
            ;;
    esac
done

# Remove the switches we parsed above.
shift `expr $OPTIND - 1`

# We want at least one non-option argument. 
# Remove this block if you don't need it.
#if [ $# -eq 0 ]; then
#    echo $USAGE >&2
#    exit 1
#fi

# Access additional arguments as usual through 
# variables $@, $*, $1, $2, etc. or using this loop:
#for PARAM; do
#    echo $PARAM
#done

#cst $debug_flag $verbose_flag output=hp-stream-table.txt basin=nanbasin stream=str.t100 dem=bigdem patch=p.dem90m.cl zone=h.t100 hill=h.t100 \
#  streamBottomWidth=2.1 streamTopWidth=1.3 streamDepth=5.5 ManningsN=0.55 maxPasses=20

# Hui's basin
cst output=nan.stream stream=str.t100 dem=bigdem patch=p.dem90m.cl zone=h.t100 hill=h.t100 basin=nanbasin ManningsN=0.05 streamTopWidth=topwidth streamBottomWidth=bottomwidth streamDepth=depth maxPasses=10


