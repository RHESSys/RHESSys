#!/bin/bash

#./searchParams.py -v --searchType=hierarchical --name="deciduous" --format=csv
#./searchParams.py -v --searchType=hierarchical --name="Deciduous" --format=param
#./searchParams.py -v --searchType=hierarchical --name="deciduous" --format=param --output=my-deciduous.def

#./searchParams.py -v --searchType=hierarchical --name="grass" --format=csv
#./searchParams.py -v --searchType=hierarchical --name="grass" --format=param
#./searchParams.py -v --searchType=hierarchical --name="grass" --format=param --output=my-grass.def

#./searchParams.py -v --searchType=hierarchical --name="Red Alder" --format=csv
#./searchParams.py -v --searchType=hierarchical --name="water" --format=csv
#./searchParams.py -v --searchType=hierarchical --name="Red Alder" --format=param
#./searchParams.py -v --searchType=hierarchical --name="Red Alder" --location="Oregon" --format=csv
#./searchParams.py -v --searchType=hierarchical --name="Red Alder" --location="Oregon" --format=param
#./searchParams.py -v --searchType=hierarchical --name="Red Alder" --location="Oregon" --format=param --output=foo.def
#./searchParams.py -v --searchType=hierarchical --name="Red Alder" --location="Oregon" --startDatetime="2013-04-15" --format=csv

#./searchParams.py -v --searchType=hierarchical --name="douglas fir" --format=csv
#./searchParams.py -v --searchType=hierarchical --name="douglas fir" --location="Oregon" --format=csv
./searchParams.py -v --searchType=hierarchical --name="douglas fir" --location="New Mexico" --format=csv

#./searchParams.py -v --searchType=constrained --location="Oregon" --format=csv
#./searchParams.py -v --searchType=constrained --genus="Alnus" --format=csv
#./searchParams.py -v --searchType=constrained --genus="Alnus" --param="snow" --format=csv
#./searchParams.py -v --searchType=constrained --param="snow" --format=csv
#./searchParams.py -v --searchType=constrained --name="Red Alder" --param="snow" --format=csv
#./searchParams.py -v --searchType=constrained --name="Red Alder" --location="" --startDatetime="2013 04 12"
#./searchParams.py -v --searchType=constrained --name="Sandy loam" --param="snow" --format=csv
#./searchParams.py -v --searchType=constrained --reference="Jones" --format=csv
# Search for parameter name by partial name, i.e. "epc" matches "epc.maxlgf", "epc.ndays_expand", ...
#./searchParams.py -v --searchType=constrained --param="epc.leaflitr_cn" --format=csv
#./searchParams.py -v --searchType=constrained --param="epc" --startDatetime="2013-04-15 12:00:00" --endDatetime="2013-04-15 17:00:00" --format=csv
#./searchParams.py -v --searchType=constrained --type="soil" --format=csv
