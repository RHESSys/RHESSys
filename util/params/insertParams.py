#!/usr/bin/env python
"""
Author : Peter Slaughter
Date   : March. 2013
Purpose: Insert parameters into the RHESsys parameter database.
"""

import commands, csv, getopt, os, re, string, sys, time
import rhessys.constants as rpc
from rhessys.params import paramDB

# Commented parameter file, i.e.
# -0.65   epc.psi_open                    # White p.52 - Average value

paramFileRegex = re.compile('^\s*([\w.-]+)\s*([\w.-]+)[#]*(.*)$')
csvFileRegex = re.compile('^\s*(\w+)\s*,(\w+).*$')
# Type of parameter: "basin", "landuse", "surface_energy", "fire", "soil", "zone", "hillslope", "stratum"
#classTypeList = ["basin", "landuse", "surface_energy", "fire", "soil", "zone", "hillslope", "stratum"]

def usage():
    print''
    print 'Program: %s' % sys.argv[0]
    print 'Purpose: insert parameters into the RHESsys parameter database'
    print ''
    print 'Syntax:'
    print '  %s --name=<class name> [--comment=<comment>] [--genus==<genus>] --location=<location> --parentName=<parent class name> [--replace] [--species=<species>] --type=<parameter type> [--user=<user>] --verbose' % sys.argv[0]
    print ''
    print '    where:'
    print '      --name=<class name>: any name that can uniquely identify a set of parameters' 
    print '             - example class names might be "Western Hemlock", "Ponderosa Pine, eastern Sierra"'
    print '      --reference=<creference>: this can be any string that the user wishes to include with each inserted record'
    print '      --genus=<genus>: the name of a genus that will be used for each inserted record'
    print '      --location=<location>: the location of the class, e.g. "New Mexico".'
    print '      --species=<species>: the name of a genuse that will be used for each inserted record'
    print '      --parentName=<parent class name>: specify the parent class name of the class you are inserting, e.g.'
    print '        if you are inserting parameters for class "Douglas Fir", the parentName is "Conifer". If'
    print '        the class being inserted does not have a parent, then you must specify "None"'
    print '      --replace: parameters in the input .def file will replace the parameter values already in the database'
    print '      --type=<parameter type>: the class type - valid types are: ', rpc.VALID_TYPES
    print '      --user=<user>: the name of the person that the inserted parameter records will be associated with'
    print ''
    print '    for example:'
    print ''
    print '        %s --type="stratum" --name="Conifer"     --parentName="None" --user="Peter Slaughter" --replace veg_conifer.def'
    print '        %s --type="stratum" --name="Douglas Fir" --parentName="Conifer" --user="Peter Slaughter" veg_douglasfir.def'
    print '        %s --type="stratum" --name="Douglas Fir" --parentName="Conifer" --location="New Mexico" --user="Peter Slaughter" veg_douglasfir_newmexico.def'
    print ''

if __name__ == '__main__':

    location = None
    parentLocation =None
    parentName=None
    user = None
    genus = None
    species = None
    className = None
    classType = None
    comment = None
    replace = False
    verbose = False
    validTypes = rpc.VALID_TYPES
    paramsDBfilename = rpc.PARAM_DB_FILENAME

    # Parse command line
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hv", ["name=", "comment=", "genus=", "location=", "parentLocation=", "parentName=", "replace", "species=", "type=", "user=", "verbose"]) 
    except getopt.GetoptError:
        # print help information and exit:
        print 'Error in command line:\n %s' % sys.argv
        print 'exception type= ', sys.exc_type
        print 'exception value= ', sys.exc_value
        usage()
        sys.exit(1)

    # Check if no options were entered, then print help and exit
    #if (len(opts) == 0):
    #    usage()
    #    sys.exit(1)

    for o, a in opts:
        if o in ("-h", "--help"):
            usage()
            sys.exit()
        elif o in ("--comment"):
            comment = a
        elif o in ("--genus"):
            genus = a
        elif o in ("--location"):
            location = a
        elif o in ("--species"):
            species = a
        elif o in ("--name"):
            className = a
        elif o in ("--parentName"):
            parentName = a
        elif o in ("--parentLocation"):
            parentLocation = a
        elif o in ("--replace"):
            replace = True
        elif o in ("--type"):
            classType = a
        elif o in ("--user"):
            user = a
        elif o in ("-v", "--verbose"):
            verbose = True

    if (len(args) < 1):
        msg = "Please include a parameter filename for input"
        raise RuntimeError, msg

    if (not classType in rpc.VALID_TYPES):
        msg = 'Please specify a type (i.e. --type=<type>) from the list: %s' % ', '.join(map(str, rpc.VALID_TYPES))
        raise RuntimeError, msg

    if (className == None):
        msg = 'Please specify a class name (i.e. --name=<class name>) for this parameter file: %s'
        raise RuntimeError, msg

    if (user == None):
        msg = 'Please specify a user (i.e. --user<Fred Smith>) for this parameter file: %s'
        raise RuntimeError, msg

    if (parentName == None):
        msg = 'Please specify a parent name (i.e. --parentName="Conifer", --parentName="None")'
        raise RuntimeError, msg

    filename = args[0]

    DBobj = paramDB()
    DBobj.insert(filename, location, classType, className, parentLocation, parentName, user, genus, species, comment, replace, verbose)
