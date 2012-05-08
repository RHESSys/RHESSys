#!/bin/bash
#
# Simple shell script that calls 'cst' program with appropriate arguments

# Run cst with basin defined - only streams inside basin will be processed
cst output=hja-stream-table.txt stream=stream dem=hjadem zone=zone hill=hillslope patch=patch basin=hjabasin

# No basin specified, entire map will be processed
#cst output=new stream=stream dem=hjadem zone=zone hill=hillslope patch=patch

# Specify input maps for stream width, depth, ManningsN
#cst output=new stream=stream dem=hjadem zone=zone hill=hillslope patch=patch basin=hjabasin streamWidth=streamWidth streamDepth=streamDepth ManningsN=ManningsN 

# Debugging
#valgrind --track-origins=yes --leak-check=full --dsymutil=yes cst output=new stream=stream dem=hjadem zone=zone hill=hillslope patch=patch basin=hjabasin
