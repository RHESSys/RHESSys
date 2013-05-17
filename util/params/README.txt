Overview
--------
The RHESsys parameter database 'params.sqlite' contains a collection of RHESsys runtime parameters contributed by the ECOHydrology lab
and other researchers.

The Python scripts in the ./util directory can be used to search for, add and delete parameters from the database. The following scripts are
currently available: insertParams.py, searchParams.py, deleteParams.py. For convenience, the BASH scripts inserParams.sh, searchParams.sh
and deleteParams.sh can be used to call the corresponsing Python script.

Initial database creation and population
----------------------------------------
In order to create the params.sqlite database for the first time, run the script createParamDB.sh, or simply copy the file empty-params.sqlite

The insertParams.py script is used to read parameters from a definition file (.def) and enter them into the database. 

Class Hierarchy
---------------
The parameter database is organized into parameter classes which corresponsds to the groupings of parameters in .def files. Typical
class names are "Deciduous", "Red Alder", "Sandy loam".

These classes are organized into a simple hierarchy, where each class may have one parent class. The class hierarchy is only
two levels deep, for example "Deciduous" is the parent class and "Red Alder" is the child. The class "Deciduous" does not
have a parent.

Classes may be specified with or without a location, for example, a set a parameters may be in the database for "Red Alder", no location
specified and a separate set of parameter may exist for "Red Alder", "Orgeon".

Hierarchical Search
-------------------
This scheme of class hierarchy and location generic or location specific classes minimizes duplication of parameters for similiar classes.
For example, it is possible to enter a full set of parameters for class "Deciduous", and then only enter parameters that are
unique to Red Alder in the class "Red Alder" (no location specified). Then for Red Alder occuring in Oregon, only unique parameter for
Orgeon are entered in the class "Red Alder", "Oregon".

When parameters are retrieved using the "hierarchical" search method, more specified classes override the general, so when retrieving
parameters for "Red Alder", "Orgeon", paraneter are retrieved for the parent class "Deciduous", then the location generic class
"Red Alder", then the location specific class "Red Alder", "Orgeon":

    - "Red Alder", "Oregon" -> overrides duplicates of "Red Alder" -> overrides duplicates of "Decidous"

The parameter "stratum_default_ID" is handled specially by the database software. This parameter used to be contained in each
.def file used by RHESsys, which requires that the value of this parameter is unique across all .def files that are used in
a RHESsys modelling run. With the use of the parameter database it is necessary for the database software to maintain a unique
number for this parameter for each class in the database. The database software therefor determines the correct value for 
this parameter when the database is searched and a parameter file is written. The value used is the class_id value for the
class that was requested during the search. For example, if we performed the search:

    ./searchParams.py -v --searchType=hierarchical --class="Red Alder" --location="Oregon" --format=param

the class_id for "Red Alder", "Orgeon" would be used (the requested class), even though other classes have contributed parameters to the output, in
this case, "Deciduous" and "Red Alder" (no location specified).

Constrained Search
------------------
An alternative to the hierarchical search is the more standard "constrained" search in which parameters are retrieved based on
a match of all the search terms in an 'and' condition, for example, the search:

   ./searchParams.py -v --searchType=constrained --genus="Alnus" --param="snow" --format=csv

returns just the parameters for genus "Alnus" and parameter names that contain the string "snow":

    Red Alder,Oregon,specific_snow_capacity,0.0015,Alnus,rubra,2013-04-15 08:56:07,,Peter Slaughter

The constrained search is intended for informational purposes only, as it may not output a complete .def file
as the hierarchical search does.


Output formats
--------------
The output formats "csv" and "param" are supported. The csv format (Comma Separated Values) gives complete information
about each parameter and the class that it belongs to. This format is intended to be used to understand what classes
contributed to a set of parameters. The "param" format is used to create files that can be read by RHESsys and 
contains only the parameter value, name and reference, if one exists for a parameter.


Ultra-quick start
-----------------
1. Use the default params.sqlite that was included in the distribution tar file
2. Edit searchParams.sh
3. Run searchParams.sh

Quickstart
----------
1. Run createParamDB.sh or copy empty-params.sqlite to params.sqlite
2. Edit insertParams.sh
3. Run insertParams.sh
4. Edit searchParams.sh
5. Run searchParams.sh

For Programmers - RHESsys Parameter Database Python API
-------------------------------------------------------

The insertParams.py, searchParams.py and deleteParams.py demonstrate the use of the Python API.

To insert values into the database:

    import rhessys.constants as rpc
    from rhessys.params import paramDB

    location = None
    parentLocation = None
    parentName = None
    user = "Peter Slaughter"
    genus = "Alnus"
    species = "rubra"
    className = "Red Alder"
    classType = "stratum"
    comment = None
    reference = None
    replace = False
    verbose = False
    paramsDBfilename = rpc.PARAM_DB_FILENAME

    DBobj = paramDB()
    DBobj.insert(filename, location, classType, className, parentLocation, parentName, user, genus, species, comment, replace, verbose)

To search the database, you can perform either a "hierarchical" search or a "constrained" search.

To perform a hierarchical search:

    import rhessys.constants as rpc
    from rhessys.params import paramDB

    comment = None
    className = None
    classType = None
    datetimeEnd = None
    datetimeStart = None
    location = None
    outputFormat = "csv"
    param = None
    genus = None
    outputPath = None
    reference = None
    searchType = None
    species = None
    user = None
    verbose = False
    DBobj = paramDB()
    DBobj.search(searchType, classType, className, location, param, genus, species, datetimeStart, datetimeEnd, user, reference)
    DBobj.write(outputPath, outputFormat)

To perform a constrained search:

    import rhessys.constants as rpc
    from rhessys.params import paramDB

    comment = None
    className = None
    classType = None
    datetimeEnd = None
    datetimeStart = None
    location = None
    outputFormat = "csv"
    param = None
    genus = None
    outputPath = None
    reference = None
    searchType = None
    species = None
    user = None
    verbose = False
    DBobj = paramDB()
    DBobj.search(searchType, classType, className, location, param, genus, species, datetimeStart, datetimeEnd, user, reference)
    DBobj.write(outputPath, outputFormat)

