#!/usr/bin/env python

"""
Author : Peter Slaughter
Date   : Feb. 2013
Purpose: Maintain database of RHESSys runtime parameters
"""

import ConfigParser
import commands, csv, getopt, os, re, string, sys, time
from datetime import datetime
from datetime import date
import logging
import time
import tempfile
import sqlite3
import rhessys.constants as rpc

paramFileRegex = re.compile('^\s*([\w.-]+)\s*([\w.-]+)#*.*$')
csvFileRegex = re.compile('^\s*(\w+)\s*,(\w+).*$')
# Type of parameter: "basin", "landuse", "surface_energy", "fire", "soil", "zone", "hillslope", "stratum"
classTypeList = ["basin", "landuse", "surface_energy", "fire", "soil", "zone", "hillslope", "stratum"]

def readPropertiesFile(filename):

    config = ConfigParser.SafeConfigParser()
    
    if (os.path.exists(filename)):
        try:
            config.read(filename)
        except:
            excInfo = sys.exc_info()
            msg = "Error reading property file %s\n" % filename
            print ("exception type= %s" % excInfo[0])
            print ("exception value= %s" % excInfo[1])
            raise RuntimeError, msg 
    else:
        msg = "Property file %s not found." % filename
        raise RuntimeError, msg 
    
    return config

def logCommand(conn):
    c = conn.cursor()
    currentTimeStr = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
    cmd = ' '.join(sys.argv)

    adminVals = (currentTimeStr, "Peter Slaughter", cmd)

    try:
        c.execute('insert into admin (dt, user, command) values (?, ?, ?)', adminVals)
    except sqlite3.IntegrityError:
        print 'exception type= %s, exception value= %s' % (sys.exc_info()[0] , sys.exc_info()[1])
   
    conn.commit()
    c.close()

def fetchClass(conn, classId, classType, className, location, genus, species):

    """ Fetch all classes that meet the specified search criteria. """

    c = conn.cursor()
    resultSet = None
    
    selectClause = 'select %s from class_type ct, class c' % rpc.TYPE_JOIN_CLASS_FIELD_NAMES_QUALIFIED 
    whereClause = ' where ct.type_id=c.type_id '

    if (className != None):
        # Have to use exact match for the class, as the search string 'clay-loam' matches
        # 'clay-loam' and 'silt-clay-loam' for a 'like' match
        #whereClause += r" and c.name like '%" + "%s" % className + r"%'"
        whereClause += r" and lower(c.name) = '%s'" % className.lower()
    elif (classId != None):
        whereClause += " and c.class_id = \'%s\'" % classId
    #else:
    #    msg = "Internal Error: must specify class name or class id"
    #    raise RuntimeError, msg

    if (classType != None):
        whereClause += " and ct.type_name = \'%s\'" % classType

    if (location != None):
        whereClause += " and lower(c.location) = \'%s\'" % location.lower()

    if (genus != None):
        whereClause += " and c.genus = \'%s\'" % genus

    if (species != None):
        whereClause += " and c.species = \'%s\'" % species
    
    queryStr = selectClause + whereClause

    try:
        c.execute(queryStr)
        resultSet = c.fetchall()
    except:
        print "Query statement: ", queryStr
        print 'exception type= ', sys.exc_type
        print 'exception value= ', sys.exc_value
        msg = "Error retrieving classes from database."
        raise RuntimeError, msg
    finally:
        c.close()

    return resultSet

def putClass(conn, className, location, classTypeId, genus, species, defaultId, parentId):

    c = conn.cursor()
    resultSet = None
    classVals = []

    # Use a null for the class_id field for the insert so that sqlite will autoincrement this field
    classVals.append(None)
    classVals.append(className)

    if (location == None):
        location = ""

    classVals.append(location)
    classVals.append(classTypeId)

    if (genus == None):
        genus = ""
    classVals.append(genus)

    if (species == None):
        species = ""
    classVals.append(species)

    if (defaultId == None):
        defaultId = getNextDefaultId(conn, classTypeId, className, location, parentId)

    classVals.append(defaultId)

    if (parentId == None):
        parentId = 0

    classVals.append(parentId)
    
    # fields: 'class_id,name,location,type_id,genus,species,default_id,parent_id' 
    try:
        c.execute('insert into class values (?, ?, ?, ?, ?, ?, ?, ?)', classVals)
    except sqlite3.IntegrityError:
        print 'exception type= %s, exception value= %s' % (sys.exc_info()[0] , sys.exc_info()[1])

    conn.commit()
    c.close()

    return

def getNextDefaultId(conn, classTypeId, className, location, parentId):
    """ Determine the value of the 'default_id' field for a new class type. """

    classType = fetchClassType(conn, classTypeId, None)
    classTypeName = classType[rpc.CLASS_TYPE_IND['type_name']]
    classTypeMaxId = int(classType[rpc.CLASS_TYPE_IND['max_id']])
    classTypeMaxId += 1

    # Get parent class info
    if (parentId != 0 and parentId != None):
        parentClass = fetchClass(conn, parentId, None, None, None, None, None)
        parentDefaultId = parentClass[0][rpc.TYPE_JOIN_CLASS_IND['default_id']]

    # For stratum class types, a child class_id must have the parent_id prepended to it.
    done = False
    while (not done):
        if (parentId == 0 or parentId == None):
            defaultId = str(classTypeMaxId)
        else:
            defaultId = str(parentDefaultId) + str(classTypeMaxId)
   
        # Check if this defaultId has already been used for this class. If we had
        # many classes, this method could result in many db accesses.
        if (defaultIdExists(conn, classTypeId, defaultId)):
            classTypeMaxId += 1
            done = False
        else:
            done = True
        
    # Update the db with the new value
    updateClassType(conn, classTypeMaxId, classTypeId)

    return defaultId

def defaultIdExists(conn, classTypeId, defaultId):

    selectClause = 'select %s from class' % rpc.CLASS_FIELD_NAMES
    whereClause = " where type_id = \'%s\' and default_id = \'%s\'" % (classTypeId, defaultId)
    queryStr = selectClause + whereClause

    c = conn.cursor()
    c.execute(queryStr)
    resultSet = c.fetchone()
    c.close()

    if resultSet == None:
        return False 
    else:
        return True

def updateClassType(conn, classTypeMaxId, classTypeId):

    updateClause = 'update class_type set max_id = %s ' % classTypeMaxId
    whereClause = ' where type_id = %s' % classTypeId
    updateStr = updateClause + whereClause

    c = conn.cursor()
    c.execute(updateStr)
    c.close()

def openParamsDB(paramsDBfilename):

    conn = sqlite3.connect(paramsDBfilename)

    return conn

def fetchBaseClass(conn, className, location):

    resultSet = None
    # Retrieve the current class
    currentClassResult = fetchClass(conn, None, None, className, location, None, None)[0]
    #if (len(currentClassResult) > 1):
    #    msg = "Duplicate base class for className: %s, location %s" % (className, location)
    #    raise RuntimeError, msg

    parentId = currentClassResult[rpc.TYPE_JOIN_CLASS_IND['parent_id']]
    # A parentId of 0 indicates that this class has no parent, so
    # return an empty resultSet
    if (parentId == 0):
        return None

    # Retrieve the parent class of className
    selectClause = 'select %s from class_type ct, class c' % rpc.TYPE_JOIN_CLASS_FIELD_NAMES_QUALIFIED
    whereClause = " where ct.type_id=c.type_id and c.class_id = \'%s\'" % parentId
    queryStr = selectClause + whereClause

    c = conn.cursor()
    c.execute(queryStr)
    resultSet = c.fetchone()
    c.close()

    #baseClassName = resultSet[rpc.TYPE_JOIN_CLASS_IND['name']]

    return resultSet

def fetchParams(conn, className, classType, location, param, genus, species, startDatetimeStr, endDatetimeStr, reference):

    c = conn.cursor()
    resultSet = None
    selectClause = 'select %s from class c, param p' % rpc.PARAM_FIELD_NAMES_QUALIFIED
    whereClause = ' where c.class_id=p.class_id '

    if (className != None):
        whereClause += r" and c.name like '%" + "%s" % className+ r"%'"

    if (location != None):
        whereClause += " and c.location = '%s'" % location
    #else:
    ##    whereClause += ' and c.location = ""'

    if (genus != None):
        whereClause += " and c.genus = '%s'" % genus

    if (param != None):
        whereClause += r" and p.name like '%" + "%s" % param + r"%'"

    if (species != None):
        whereClause += " and c.species = '%s'" % species

    if (startDatetimeStr != None):
        whereClause += " and p.dt >= '%s'" % startDatetimeStr

    if (endDatetimeStr != None):
        whereClause += " and p.dt <= '%s'" % endDatetimeStr

    if (reference != None):
        whereClause += r" and p.reference like '%" + "%s" % reference + r"%'"

    queryStr = selectClause + whereClause

    try:
        c.execute(queryStr)
        resultSet = c.fetchall()
        c.close()
    except:
        print "Query statement: ", queryStr
        print 'exception type= ', sys.exc_type
        print 'exception value= ', sys.exc_value
        msg = "Error retrieving parameters from database."
        raise RuntimeError, msg

    return resultSet

def filterDuplicates(resultSet, paramInd):
    # Sort the result set by paramInd (usually parameter name)
    resultSet = sorted(resultSet, key=lambda param: param[paramInd]) 
    outResultSet = []

    # Now look for duplicates and use the best one, discarding other duplicates.
    # Currently the only criteria to eliminate dups within a class are the last
    # insertion date.
    for resultInd in range(0, len(resultSet)):
       if (resultInd == len(resultSet) - 1):
           outResultSet.append(resultSet[resultInd])
           continue

       name1 = resultSet[resultInd][rpc.PARAM_IND['name']]
       name2 = resultSet[resultInd+1][rpc.PARAM_IND['name']]

       if (name1.lower() == name2.lower()):
           val1 = resultSet[resultInd][rpc.PARAM_IND['value']]
           val2 = resultSet[resultInd+1][rpc.PARAM_IND['value']]

           dt1 = resultSet[resultInd][rpc.PARAM_IND['dt']]
           dt2 = resultSet[resultInd+1][rpc.PARAM_IND['dt']]

           # If the insertion dates are the same, use the last entry, otherwise
           # use the last inserted value
           if (dt1 == dt2):
               result = resultSet[resultInd+1]
           elif (dt1 > dt2):
               result = resultSet[resultInd]
       else:
           result = resultSet[resultInd]

       outResultSet.append(resultSet[resultInd])

    return outResultSet

def overlayResultSet(paramResult, overlayResult):
    """ Overlay the second resultSet on top of the first resultSet """
      
    # Check if one or the other of the results sets are empty, and if yes
    # then return the other. It's fine if the returned result is also empty.
    if (len(paramResult) == 0):
        return overlayResult
    elif (len(overlayResult) == 0):
        return paramResult
 
    # Index of the list element we are using for comparison
    paramNameInd = rpc.PARAM_IND['name']

    # Both 'paramResult' and 'overlayResult' are query resultSets obtained from
    # the 'fetchParams' method.

    # Sort the result sets by parameter name
    paramResult = sorted(paramResult, key=lambda param: param[paramNameInd]) 
    overlayResult = sorted(overlayResult, key=lambda param: param[paramNameInd]) 

    # Perform a simple merge on the sorted lists, replacing any duplicates with the list element
    # from the overlay list

    finalResultSet = []
    paramInd = 0
    overlayInd = 0
    paramLen = len(paramResult)
    overlayLen = len(overlayResult)
  
    while (paramInd < paramLen or overlayInd < overlayLen):
        # Reached the end of the 1st resultSet so continue outputting overlay result
        if (paramInd == paramLen):
            finalResultSet.append(overlayResult[overlayInd])
            overlayInd += 1
            continue

        # Reached the end of overlay result so continue outputing 1st result
        if (overlayInd == overlayLen):
            finalResultSet.append(paramResult[paramInd])
            paramInd += 1
            continue
 
        overlayParamName = overlayResult[overlayInd][paramNameInd]
        paramName = paramResult[paramInd][paramNameInd]

        # The param from the first list can either be equal to, less than or greater than
        # the param in the overlay list.
        if (paramName == overlayParamName):
            finalResultSet.append(overlayResult[overlayInd])
            paramInd += 1
            overlayInd += 1
        elif (paramName < overlayParamName):
            finalResultSet.append(paramResult[paramInd])
            paramInd += 1
        elif (paramName > overlayParamName):
            finalResultSet.append(overlayResult[overlayInd])
            overlayInd += 1
        else:
            msg = "Internal Error"
            raise RuntimeError, msg
        
    return finalResultSet

def writeClass(conn, classes, outputPath):

    #currentDatetimeStr = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())

    if (outputPath != sys.stdout):
        try:
            fdOut = open(outputPath, 'w')
        except:
            msg = "Unable to open class info output file %s" % outputPath
            raise RuntimeError
    else:
        fdOut = sys.stdout

    #fdOut.write(rpc.CLASS_FIELD_NAMES)
    fdOut.write('class type,name,location,genus,species,default id,parent id\n')
    paramsCSVwriter = csv.writer(fdOut, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)

    #print "classes: ", classes
    for c in sorted(classes, key=lambda classes: classes[rpc.TYPE_JOIN_CLASS_IND['name']]):
        (type_id,type_name,max_id,class_id,name,location,type_id,genus,species,default_id,parent_id ) = c
        outLine = (type_name,name,location,genus,species,default_id,parent_id)

        paramsCSVwriter.writerow(outLine)

    fdOut.close()
   
def writeCSV(conn, params, classes, outputPath):

    if (outputPath != sys.stdout):
        try:
            fdOut = open(outputPath, 'w')
        except:
            msg = "Unable to open params output file %s" % outputPath
            raise RuntimeError
    else:
        fdOut = sys.stdout

    #currentDatetimeStr = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
    fdOut.write("class,location,name,value,genus,species,date,reference,user\n")
    paramsCSVwriter = csv.writer(fdOut, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)

    for p in sorted(params, key=lambda param: param[rpc.PARAM_IND['name']]):
        (class_id, name, value, dt, reference, comment, user) = p
        if (class_id not in classes.keys()):
            paramClass = fetchClass(conn, class_id, None, None, None, None, None)[0]
        else:
            paramClass = classes[class_id]

        className = paramClass[rpc.TYPE_JOIN_CLASS_IND['name']]
        location  = paramClass[rpc.TYPE_JOIN_CLASS_IND['location']] 
        genus     = paramClass[rpc.TYPE_JOIN_CLASS_IND['genus']]
        species   = paramClass[rpc.TYPE_JOIN_CLASS_IND['species']]
        outLine   = (className, location, name, value, genus, species, dt, reference, user)

        paramsCSVwriter.writerow(outLine)

    fdOut.close()
    
def writeParams(conn, params, outputPath):

    if (outputPath != sys.stdout):
        try:
            fdOut = open(outputPath, 'w')
        except:
            msg = "Unable to open params output file %s" % outputPath
            raise RuntimeError
    else:
        fdOut = sys.stdout

    currentTimeStr = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())

    for p in sorted(params, key=lambda param: param[rpc.PARAM_IND['name']]):
        classId, name, value, dt, reference, comment, user = p
        fdOut.write("%s %s %s\n" % (value, name, comment))

    fdOut.close()

def fetchClassType(conn, classTypeId, classTypeName):
    """ Fetch a classType entry from the 'class_type' table. """

    c = conn.cursor()
    
    selectClause = 'select %s from class_type ct' % rpc.CLASS_TYPE_FIELD_NAMES_QUALIFIED
    if (classTypeId != None):
        whereClause = " where ct.type_id = \'%s\'" % classTypeId
    elif (classTypeName != None):
        whereClause = " where ct.type_name = \'%s\'" % classTypeName
    else:
        msg = "Internal error: must specify classTypeId or classTypeName when fetching classId entry"
        raise RuntimeError

    queryStr = selectClause + whereClause

    try:
        c.execute(queryStr)
        result = c.fetchone()
        c.close()
    except:
       print "Query statement: ", queryStr
       print 'exception type= ', sys.exc_type
       print 'exception value= ', sys.exc_value
       msg = "Error retrieving class type from database."
       raise RuntimeError, msg

    return result

def openParamsDB(paramsDBfilename):

    conn = sqlite3.connect(paramsDBfilename)

    return conn

def createParamsDB(paramsDBfilename, conn):

    # Create the RHESsys parameter database
    cmd = 'sqlite3 %s < createParamDB.sql' % rpc.PARAM_DB_FILENAME

    (status,cmdOutput) = commands.getstatusoutput(cmd)

    if (debug):
        if (status):
            raise RuntimeError, 'Command failed: %s' % cmdOutput
        else:
            print 'command successful: %s' % cmd

def readParamFile(filename):

    try:
        fp = open(filename, "r")
    except:
        print 'exception type= ', sys.exc_type
        print 'exception value= ', sys.exc_value
        msg = "Unable to open input parameter file %s" % filename
        raise RuntimeError

    params = []

    for line in fp:
        if (line.strip() == ""):
            continue
        m = paramFileRegex.match(line)
        #print m.groups()
        paramVal = m.groups()[0].replace('\r', '')
        paramName = m.groups()[1].replace('\r', '')
        # Filter out the parameter 'stratum_default_id' because
        # this parameter value is now maintained by the Python parameter
        # database software.
        if (paramName.lower() == "stratum_default_id"):
            continue

        if (len(m.groups()) > 2):
           paramRef = m.groups()[2].lstrip().replace('\r', '')
        else:
           paramRef = ""

        params.append((paramVal, paramName, paramRef))

    fp.close()

    return params

def readCSVfile(filename):

    paramReader = csv.reader(open(filename, 'rbU'), delimiter=',', quotechar='"')
    fields = paramReader.next()
    line = ','.join(fields)

class paramDB:

    def __init__(self):

        if (not os.path.exists(rpc.PARAM_DB_FILENAME)):
            msg = "Parameter database %s not found" % rpc.PARAM_DB_FILENAME
            raise RuntimeError, msg

        self.conn = sqlite3.connect(rpc.PARAM_DB_FILENAME)
        self.params = []
        self.classes = {}
        self.requestedClass = None
        self.requestedClassId = None
        self.searchResultType = None
        
    def searchHierarchical(self, classType, className, location, param, genus, species, user, startDatetimeStr, endDatetimeStr, reference):

        self.searchResultType = "param"

        # fetch the classId of the requested class
        requestedClass = fetchClass(self.conn, None, None, className, location, None, None)
        if (len(requestedClass) == 0):
            msg = "Class name \"%s\", location \"%s\" not found." % (className, location)
            raise RuntimeError, msg

        self.requestedClassId = requestedClass[0][rpc.TYPE_JOIN_CLASS_IND['class_id']]

        # fetch parameters for base class if it exists
        baseClassParams = []
        baseClass = fetchBaseClass(self.conn, className, location)
        print "base: ", baseClass
        # No base class exists for this class, but it still may have a location generic class
        if (baseClass != None):
            baseClassName = baseClass[rpc.TYPE_JOIN_CLASS_IND['name']]
            baseClassId = baseClass[rpc.TYPE_JOIN_CLASS_IND['class_id']]

            # Capture the class_id for output later
            if (baseClassId not in self.classes):
                self.classes[baseClassId] = baseClass

            baseClassParams = []
            if (baseClassName != None):
                baseClassParams = fetchParams(self.conn, baseClassName, None, None, None, None, None, startDatetimeStr, endDatetimeStr, None) 
                #baseClassParams = filterDuplicates(baseClassParams, rpc.PARAM_IND['name'])

        # retrieve the "generic" version of the requested class
        genericClassParams = fetchParams(self.conn, className, None, "", None, None, None, startDatetimeStr, endDatetimeStr, None) 

        # Sort and filter duplicates for the generic class parameters
        if (len(genericClassParams) > 0):
            genericClass = fetchClass(self.conn, None, None, className, None, None, None)[0]
            genericClassId = genericClass[rpc.TYPE_JOIN_CLASS_IND['class_id']]
            if (genericClassId not in self.classes):
                self.classes[genericClassId] = genericClass
            
            # Sort parameter resultSet by parameter name
            #genericClassParams  = filterDuplicates(genericClassParams, rpc.PARAM_IND['name'])
         
        # Overlay generic parameters on base class parameters
        finalParams = overlayResultSet(baseClassParams, genericClassParams)

        # Fetch parameters for the location specific class (if requested)
        if (location != None):
            locationSpecificParams = fetchParams(self.conn, className, None, location, None, None, None, startDatetimeStr, endDatetimeStr, None) 
            locationSpecificClass = fetchClass(self.conn, None, None, className, location, None, None)[0]
            locationSpecificClassId = locationSpecificClass[rpc.TYPE_JOIN_CLASS_IND['class_id']]

            if (locationSpecificClassId not in self.classes):
                self.classes[locationSpecificClassId] = locationSpecificClass

            # Resolve any duplicates 
            #locationSpecificParams = filterDuplicates(locationSpecificParams, rpc.PARAM_IND['name'])
            finalParams = overlayResultSet(finalParams, locationSpecificParams)

        if (self.requestedClassId != None):
            currentDatetimeStr = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
            finalParams.append([self.requestedClassId, "stratum_default_ID", self.requestedClassId, currentDatetimeStr, "", "", ""])

        return finalParams

    def searchConstrained(self, classType, className, location, param, genus, species, user, startDatetimeStr, endDatetimeStr, reference):

        self.searchResultType = "param"

        # fetch the classId of the requested class
        if (className != None):
            requestedClass = fetchClass(self.conn, None, None, className, location, None, None)[0]

            if (requestedClass == None):
                msg = "Class name \"%s\", location \"%s\" not found." % (className, location)
                raise RuntimeError, msg

            self.requestedClassId = requestedClass[rpc.TYPE_JOIN_CLASS_IND['class_id']]

        # fetch a resultset with the specified constraints
        # return resultSet
        finalParams = []
        finalParams = fetchParams(self.conn, className, classType, location, param, genus, species, startDatetimeStr, endDatetimeStr, reference)
        if (className != None):

            finalClass = fetchClass(self.conn, None, None, className, location, None, None)[0]
            finalClassId = finalClass[rpc.TYPE_JOIN_CLASS_IND['class_id']]

            if (finalClassId not in self.classes):
               self.classes[finalClassId] = finalClass

        #if (self.requestedClassId != None):
        #    currentDatetimeStr = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
        #    finalParams.append([self.requestedClassId, "stratum_default_ID", self.requestedClassId, currentDatetimeStr, "", "", ""])

        return finalParams

    def search(self, searchType, classType, className, location, param, genus, species, startDatetimeStr, endDatetimeStr, user, reference):

        self.searchResultType = "param"

        if (searchType == rpc.SEARCH_TYPE_HIERARCHICAL):
            self.params = self.searchHierarchical(classType, className, location, param, genus, species, user, startDatetimeStr, endDatetimeStr, reference)
        elif (searchType == rpc.SEARCH_TYPE_CONSTRAINED):
            self.params = self.searchConstrained(classType, className, location, param, genus, species, user, startDatetimeStr, endDatetimeStr, reference)
        else:
            msg = "Unknown search type %s" % searchType
            usage()
            raise RuntimeError, msg

    def searchClass(self, classId, classType, className, location, genus, species):

        self.searchResultType = "class"

        if (classType != None):
            if (not classType.lower() in rpc.VALID_TYPES):
                msg = 'Invalid class type \"%s\".' % classType 
                raise RuntimeError, msg

        self.classes = fetchClass(self.conn, classId, classType, className, location, genus, species)
        
    def write(self, outputPath, outputFormat):

        # If we performed a class search, print out class info
        if (self.searchResultType == "class"):
            writeClass(self.conn, self.classes, outputPath)
        # Otherwise print out param info
        else:
            if (len(self.params) == 0):
                sys.stderr.write("No parameter entries found.\n")
                return

            if (outputPath == None):
                outputPath = sys.stdout

            if (outputFormat == rpc.OUTPUT_FORMAT_CSV):
                writeCSV(self.conn, self.params, self.classes, outputPath)
            elif (outputFormat == rpc.OUTPUT_FORMAT_PARAM):
                writeParams(self.conn, self.params, outputPath)

    def insert(self, filename, location, classType, className, parentLocation, parentName, user, genus, species, comment, replace, verbose):
        """ Insert parameters into the parameter database.
            If this class name and location is new, insert it into the 'class' table, then insert parameters into the 'param' table.
        """
    
        logCommand(self.conn)
        if (classType != None):
            classType = classType.strip()

        className = className.strip()

        if (location != None): 
            location = location.strip() 
        else:
            location = ""

        # Normalize the parent name from the user input
        if (parentName.lower() == "none"):
            parentName = None

        if (parentName != None): 
            parentName = parentName.strip()

        if (parentLocation != None): 
            parentLocation = parentLocation.strip()
        else:
            parentLocation = ""

        # Here are the possible types of updates that are supported:
        # - insert a new base class, e.g. one that doesn't have a parent, but might be a parent of future classes
        # - insert a new class that has a parent (parent must have been previously inserted)
        # - add parameters to an existing class
    
        c = self.conn.cursor()
        params = readParamFile(filename)
    
        # Check if class type is valid, e.g. "basin, fire,..., zone" 
        if (not classType.lower() in rpc.VALID_TYPES):
            msg = 'Invalid class type \"%s\".' % classType 
            raise RuntimeError, msg
        else:
            classTypeId = fetchClassType(self.conn, None, classType)[rpc.CLASS_TYPE_IND['type_id']]

        # Check if an entry for this class has been entered before and if not, add it.
        classResult = fetchClass(self.conn, None, None, className, location, None, None)
    
        # The class is new, so we must fetch the parentId specified from the command line, so that
        # we can insert the parent classId with this new class.
        if (len(classResult) == 0):
            # If a parent class was specified, then retrieve it first (location is optional).
            if (parentName != None):
                parentClassResult = fetchClass(self.conn, None, None, parentName, parentLocation, None, None)
                if (len(parentClassResult) == 0):
                    msg = "Parent class=%s, location=%s not found." % (parentName, parentLocation)
                    raise RuntimeError, msg
                parentId = parentClassResult[0][rpc.TYPE_JOIN_CLASS_IND['class_id']]
            else:
                parentId = 0
    
            try:
                # Insert the new class into the class table. The value for the 'default_id' field will be
                # calculated by putClass if 'None' is specified
                defaultId = None
                putClass(self.conn, className, location, classTypeId, genus, species, defaultId, parentId)
            except:
                print 'exception type= ', sys.exc_type
                print 'exception value= ', sys.exc_value
                msg = "Error inserting class: %s, location: %s into database." % (className, location)
                raise RuntimeError, msg

            # Fetch the new classId that was assigned by the db (autoincrement) of the class we just
            # inserted.
            classResult = fetchClass(self.conn, None, None, className, location, None, None)
            if (len(classResult) == 0):
                msg = "Parent class: %s, location: not found." %s (parentName, parentLocation)
                raise RuntimeError, msg
            
            classId = classResult[0][rpc.TYPE_JOIN_CLASS_IND['class_id']]
    
            if (classResult == None):
                c.close()
                msg = "Error inserting class (%s, %s) into database" % (className, location)
                raise RuntimeError, msg
        else:
            # The class already exists, so we know the classId and parentId
            parentId = classResult[0][rpc.TYPE_JOIN_CLASS_IND['parent_id']]
            classId = classResult[0][rpc.TYPE_JOIN_CLASS_IND['class_id']]
    
        # Now insert the parameters for this class into the 'param' table
        for p in params:
            currentTimeStr = time.strftime("%Y-%m-%d %H:%M:%S", time.localtime())
            # Create a tuple of the values to insert
            pVal, pName, pReference = p
    
            #get class id from name, genus, species, location
            paramList = []
            
            paramList.append(classId)
            paramList.append(pName)
            paramList.append(pVal)
            paramList.append(currentTimeStr)
            paramList.append(pReference.replace('\r', ''))
    
            if (comment == None):
                comment = ""
            paramList.append(comment)
    
            if (user == None):
                user = ""
            paramList.append(user)

            paramVals = tuple(paramList)
            # Store time values as "YYYY-MM-DD HH:MM:SS.SSS"
            try:
                if (replace):
                    if verbose:
                        print "Inserting/replacing: ", paramVals
                    c.execute('insert or replace into param values (?, ?, ?, ?, ?, ?, ?)', paramVals)
                else:
                    if verbose:
                        print "Inserting: ", paramVals
                    c.execute('insert into param values (?, ?, ?, ?, ?, ?, ?)', paramVals)
            except sqlite3.IntegrityError:
                print 'exception type= %s, exception value= %s' % (sys.exc_info()[0] , sys.exc_info()[1])
    
        self.conn.commit()
        c.close()

    def delete(self, searchType, classType, className, location, param, genus, species, startDatetimeStr, endDatetimeStr, user, reference):

        logCommand(self.conn)

        self.search(searchType, classType, className, location, param, genus, species, startDatetimeStr, endDatetimeStr, user, reference)

        if (len(self.params) == 0):
            print "There are no parameter entries to delete for the specified search criteria"
            return
        else:
            print "The following parameter entries will be deleted: "

        paramsCSVwriter = csv.writer(sys.stdout, delimiter=',', quotechar='"', quoting=csv.QUOTE_MINIMAL)
        paramsSelected = self.params
        paramsToDelete = []

        for p in sorted(paramsSelected, key=lambda param: param[rpc.PARAM_IND['name']]):
            (class_id, name, value, dt, reference, comment, user) = p
            paramsToDelete.append((class_id, name, value))
            
            if (class_id  not in self.classes):
                paramClass = fetchClass(self.conn, class_id, None, None, None, None, None)[0]
                self.classes[class_id] = paramClass
            else:
                paramClass = self.classes[class_id]

            className = paramClass[rpc.TYPE_JOIN_CLASS_IND['name']]
            location  = paramClass[rpc.TYPE_JOIN_CLASS_IND['location']] 
            genus     = paramClass[rpc.TYPE_JOIN_CLASS_IND['genus']]
            species   = paramClass[rpc.TYPE_JOIN_CLASS_IND['species']]
            outLine   = (className, location, name, value, genus, species, dt, reference, user)
            paramsCSVwriter.writerow(outLine)

        sys.stdout.write("Do you wish to delete these parameter entries? (Y/N): ")
        answer = sys.stdin.readline().strip("\n")
        dCount = 0
        if (answer.lower() == "y"):
            c = self.conn.cursor()

            for p in paramsToDelete:
                try:
                    (class_id, name, value) = p
                    c.execute("delete from param where class_id='%s' and name='%s' and value='%s'" % (class_id, name, value))
                    dCount += 1
                except:
                #except sqlite3.IntegrityError:
                    print 'exception type= %s, exception value= %s' % (sys.exc_info()[0] , sys.exc_info()[1])
        
            self.conn.commit()
            c.close()
            print("%d records deleted." % dCount)
        else:
           sys.stdout.write("Parameters not deleted.\n")

    def searchLog(self, startDatetimeStr, endDatetimeStr, user):

        c = self.conn.cursor()
        resultSet = None
        selectClause = 'select dt,command,user from admin '
        whereClause = None
    
        if (startDatetimeStr != None):
            if (whereClause == None):
                whereClause = " where dt >= '%s'" % startDatetimeStr
            else:
                whereClause += " and dt >= '%s'" % startDatetimeStr
    
        if (endDatetimeStr != None):
            if (whereClause == None):
                whereClause = " where dt <= '%s'" % endDatetimeStr
            else:
                whereClause += " and dt <= '%s'" % endDatetimeStr
    
        if (user != None):
            if (whereClause == None):
                whereClause = r" where user like '%" + "%s" % user + r"%'"
            else:
                whereClause += r" and user like '%" + "%s" % user + r"%'"
    
        if (whereClause == None):
            queryStr = selectClause
        else:
            queryStr = selectClause + whereClause
    
        try:
            c.execute(queryStr)
            resultSet = c.fetchall()
            c.close()
        except:
            print "Query statement: ", queryStr
            print 'exception type= ', sys.exc_type
            print 'exception value= ', sys.exc_value
            msg = "Error retrieving parameters from database."
            raise RuntimeError, msg
    
        for r in resultSet:
            dt, cmd, user = r
            print "%s: %s (%s)" % (dt, cmd, user)

        return resultSet
