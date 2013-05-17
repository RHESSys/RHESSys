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
    print 'Purpose: Search the RHESsys parameter database log.'
    print ''
    print 'Syntax:'
    print '  %s [--endDatetime=<datetime>] [--startDatetime=<datetime>] [--user=<user name>] --verbose' % sys.argv[0]
    print ''
    print '    where:'
    print '      --endDatetime=<datetime>: select parameters that were inserted into the database before the specified datetime. Datetime can'
    print '        be specified in one of the formats: "YYYY-MM-DD", "YYYY-MM-DD HH:MM" or "YYYY-MM-DD HH:MM:SS", for example "2013-04-15 10:25:00"'
    print '      --startDatetime=<datetime>: select parameters that were inserted into the database after the specified datetime. Datetime can'
    print '        be specified in one of the formats: "YYYY-MM-DD", "YYYY-MM-DD HH:MM" or "YYYY-MM-DD HH:MM:SS", for example "2013-04-15 10:25:00"'
    print '      --user=<user name>: the name of the person that will be used for searching'
    print ''
    print '    for example:'
    print ''
    print '        %s --startDatetime="2013-04-16"' % sys.argv[0]
    print ''
    
if __name__ == '__main__':

    endDatetimeStr = None
    startDatetimeStr = None
    user = None
    verbose = False

    # Parse command line
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hv", ["endDatetime=", "startDatetime=", "user=", "verbose"]) 
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
        elif o in ("--startDatetime"):
            startDatetimeStr = a
        elif o in ("--endDatetime"):
            endDatetimeStr = a
        elif o in ("--user"):
            user = a
        elif o in ("-v", "--verbose"):
            verbose = True

    DBobj = paramDB()
    DBobj.searchLog(startDatetimeStr, endDatetimeStr, user)
