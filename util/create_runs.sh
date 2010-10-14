#!/bin/bash

awkfile=$1
name=$2
dir=$3
parprefix=$4

numpars=`ls $parprefix* | wc -w`

for (( i=1; i <= $numpars; i++)) do
	awk -f $awkfile name=$name dir=$dir tag=$i < "$parprefix""$i" > "$name""$i"
done

chmod +x $name*
