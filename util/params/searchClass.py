#!/usr/bin/env python

"""
Author : Peter Slaughter
Date   : Feb. 2013
Purpose: Maintain database of RHESsys runtime parameters
"""

import commands, csv, getopt, os, re, string, sys, time
import rhessys.constants as rpc
from rhessys.params import paramDB

paramFileRegex = re.compile('^\s*([\w.-]+)\s*([\w.-]+)#*.*$')
csvFileRegex = re.compile('^\s*(\w+)\s*,(\w+).*$')

def usage():
    print''
    print 'Program: %s' % sys.argv[0]
    print 'Purpose: Search the RHESsys parameter database and output the results'
    print ''
    print 'Syntax:'
    print '  %s --name=<class name> [--genus=<genus name>] --location=<location name> [--species=<species name>] [--type=<type name>] --verbose' % sys.argv[0]
    print ''
    print '    where:'
    print '      --name=<class name>: any name that can uniquely identify a set of parameters' 
    print '             - example class names might be "Western Hemlock", "Ponderosa Pine"'
    print '      --genus=<genus name>: the name of a genus that will be used for searching'
    print '      --location=<location name>: the location to search for, e.g. "Orgeon"'
    print '      --species=<species name">: the name of a species that will be used for searching'
    print ''
    print '    for example:'
    print ''
    print '        %s --name="Red Alder" --location="Oregon"' % sys.argv[0]
    print ''
    
if __name__ == '__main__':

    className = None
    classType = None
    classId = None
    location = None
    genus = None
    species = None
    verbose = False
    outputFormat = rpc.OUTPUT_FORMAT_CSV
    outputPath = sys.stdout

    # Parse command line
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hv", ["genus=", "location=", "species=", "type=", "name=", "verbose"]) 
    except getopt.GetoptError:
        # print help information and exit:
        print 'Error in command line:\n %s' % sys.argv
        print 'exception type= ', sys.exc_type
        print 'exception value= ', sys.exc_value
        usage()
        sys.exit(1)

    if len(opts) == 0:
        usage()
        sys.exit()

    for o, a in opts:
        if o in ("-h", "--help"):
            usage()
            sys.exit()
        elif o in ("--genus"):
            genus = a
        elif o in ("--species"):
            species = a
        elif o in ("--type"):
            classType = a
        elif o in ("--location"):
            location = a
        elif o in ("--name"):
            className = a
        elif o in ("-v", "--verbose"):
            verbose = True

    if (not outputFormat in rpc.VALID_FORMATS):
        msg = "Output format must be one of the following: ", rpc.VALID_FORMATS
        raise RuntimeError, msg

    DBobj = paramDB()
    DBobj.searchClass(classId, classType, className, location, genus, species)
    DBobj.write(outputPath, outputFormat)
