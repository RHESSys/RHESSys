#!/usr/bin/env python
"""@package dumpParams

@brief Perform a search against RHESSys param DB, dumping parameters for each 
class returned into a separate file in the specified directory. 

This software is provided free of charge under the New BSD License. Please see
the following license information:

Copyright (c) 2013, University of North Carolina at Chapel Hill
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the University of North Carolina at Chapel Hill nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE UNIVERSITY OF NORTH CAROLINA AT CHAPEL HILL
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


@author Brian Miles <brian_miles@unc.edu>
"""
import os, sys, errno
import argparse

import rhessys.constants as rpc
from rhessys.params import paramDB

parser = argparse.ArgumentParser(description='Perform a search against RHESSys param DB, dumping parameters for each  class returned into a separate file in the specified directory.')
parser.add_argument('--startDatetime', dest='startDatetimeStr', required=False,
                    help='select parameters that were inserted into the database after the specified datetime. Datetime can be specified in one of the formats: "YYYY-MM-DD", "YYYY-MM-DD HH:MM" or "YYYY-MM-DD HH:MM:SS", for example "2013-04-15 10:25:00"')
parser.add_argument('--endDatetime', dest='endDatetimeStr', required=False,
                    help='select parameters that were inserted into the database before the specified datetime. Datetime can be specified in one of the formats: "YYYY-MM-DD", "YYYY-MM-DD HH:MM" or "YYYY-MM-DD HH:MM:SS", for example "2013-04-15 10:25:00"')
parser.add_argument('--comment', dest='comment', required=False,
                    help='Any comment that will be used to search for matching records in the parameter database; use quotes around comments that contain spaces.')
parser.add_argument('--format', dest='outputFormat', required=False, choices=rpc.VALID_FORMATS, default=rpc.OUTPUT_FORMAT_PARAM,
                    help='Valid formats are "csv" and "param". If "csv" is specified, all fields from the database are written to the output file, separated by commas; if "param" is specified, then name, value, comment fields are output with a format that is suitable for use as a RHESsys parameter file')
parser.add_argument('--genus', dest='genus', required=False,
                    help='Name of a genus that will be used for searching')
parser.add_argument('--species', dest='species', required=False,
                    help='Name of a genus that will be used for searching')
parser.add_argument('--type', dest='classType', required=False, choices=rpc.VALID_TYPES,
                    help='Type of parameter to search for' )
parser.add_argument('--location', dest='location', required=False,
                    help='Location to search for, e.g. "Oregon"')
parser.add_argument('--name', dest='className', required=False,
                    help='Name that can uniquely identify a set of parameters. Example class names might be "Evergreen", "sandyloam", Western Hemlock", "Ponderosa Pine, eastern Sierra"')
parser.add_argument('-o', '--outputPath', dest='outputPath', required=False,
                    help='Directory to which class parameter files will be written')
parser.add_argument('--param', dest='param', required=False,
                    help='Name of the parameter to search for')
parser.add_argument('--reference', dest='reference', required=False,
                    help='Reference (citation) that will be used to search for matching records in the parameter database')
parser.add_argument('--searchType', dest='searchType', required=True, choices=rpc.SEARCH_TYPES,
                    help='Type of search to perform' )
parser.add_argument('--user', dest='user', required=False,
                    help='Name of the user who created the parameter/class')
parser.add_argument('-v', '--verbose', dest='verbose', action='store_true',
                    help='Write verbose output')
parser.add_argument('-b', '--limittobaseclasses', dest='limitToBaseClasses', action='store_true',
                    help='Limit classes returned to base classes')
args = parser.parse_args()

outputPath = None
if args.outputPath:
    if not os.access(args.outputPath, os.W_OK):
        raise IOError(errno.EACCES, "Unable write to output directory %s" % (args.outputPath,) )
    if not os.path.isdir(args.outputPath):
        raise IOError(errno.ENOTDIR, "Output path %s is not a directory" % (args.outputPath,) )
    outputPath = os.path.abspath(args.outputPath)

DBobj = paramDB()
DBobj.search(args.searchType, args.classType, args.className, args.location, args.param, args.genus, args.species, args.startDatetimeStr, args.endDatetimeStr, args.user, args.reference, args.limitToBaseClasses)
DBobj.writeParamFiles(outputPath)