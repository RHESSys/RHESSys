#!/bin/bash
#
# Simple shell script that calls 'cst' program with appropriate arguments

# Run cst with basin specified - only streams inside basin will be processed
#cst output=hja-stream-table.txt stream=stream dem=hjadem zone=zone hill=hillslope patch=patch basin=hjabasin

# No basin specified, entire map will be processed
#cst output=new stream=stream dem=hjadem zone=zone hill=hillslope patch=patch

# Specify input maps for stream width, depth, ManningsN
#cst -q -v output=new stream=stream dem=hjadem zone=zone hill=hillslope patch=patch basin=hjabasin streamBottomWidth=streamBottomWidth streamTopWidth=streamTopWidth streamDepth=streamDepth ManningsN=ManningsN 

# Specify streamBottomWidth, streamTopWidth, streamDepth, ManningsN as constants
cst -q -v output=new stream=stream dem=hjadem zone=zone hill=hillslope patch=patch basin=hjabasin streamBottomWidth=2.1 streamTopWidth=1.3 streamDepth=5.5 ManningsN=0.55 maxPasses=3

# Debugging with valgrind
#valgrind --track-origins=yes --leak-check=full --dsymutil=yes cst output=new stream=stream dem=hjadem zone=zone hill=hillslope patch=patch basin=hjabasin
#valgrind --track-origins=yes --leak-check=full --dsymutil=yes cst output=stream.nxh stream=str.t100 dem=bigdem patch=p.dem90m.cl zone=h.t100 hill=h.t100
