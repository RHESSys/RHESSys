import os, errno
from shutil import rmtree
import filecmp
from zipfile import ZipFile
from unittest import TestCase

from grassdatalookup import GRASSConfig
from grassdatalookup import GrassDataLookup

## Unit tests
class TestCF9vCF10(TestCase):
    
    @classmethod
    def setUpClass(cls):
        cfBin = os.path.join( './', os.environ['CF_BIN'] )
        cls.cfBinPath = os.path.abspath(cfBin)
        if not os.access(cls.cfBinPath, os.X_OK):
            raise IOError(errno.ENOEXEC, "Unable to execute CF binary %s" %
                          cls.cfBinPath)
        
        # We zip the GRASSData and RHESSys folders to be nice to GitHub, unzip it
        cls.grassDBasePath = os.path.abspath('./test/data/GRASSData')
        grassDBaseZip = "%s.zip" % (cls.grassDBasePath,)
        if not os.access(grassDBaseZip, os.R_OK):
            raise IOError(errno.EACCES, "Unable to read GRASS data zip %s" %
                      grassDBaseZip)
        grassDBaseDir = os.path.split(grassDBaseZip)[0]
        if not os.access(grassDBaseDir, os.W_OK):
            raise IOError(errno.EACCES, "Unable to write to GRASS data parent dir %s" %
                          grassDBaseDir)
        zip = ZipFile(grassDBaseZip, 'r')
        extractDir = os.path.split(cls.grassDBasePath)[0]
        zip.extractall(path=extractDir)
        
        cls.rhessysPath = os.path.abspath('./test/data/RHESSys')
        rhessysZip = "%s.zip" % (cls.rhessysPath,)
        if not os.access(rhessysZip, os.R_OK):
            raise IOError(errno.EACCES, "Unable to read RHESSys data zip %s" %
                      rhessysZip)
        rhessysDir = os.path.split(rhessysZip)[0]
        if not os.access(rhessysDir, os.W_OK):
            raise IOError(errno.EACCES, "Unable to write to RHESSys data parent dir %s" %
                          rhessysDir)
        zip = ZipFile(rhessysZip, 'r')
        extractDir = os.path.split(cls.rhessysPath)[0]
        zip.extractall(path=extractDir)
        
        gisbase = os.environ['GISBASE']
        grassConfig = GRASSConfig(gisbase=gisbase, dbase=cls.grassDBasePath, location='DR5_5m', mapset='taehee')
        cls.grassdatalookup = GrassDataLookup(grass_config=grassConfig)
    
    @classmethod
    def tearDownClass(cls):
        rmtree(cls.grassDBasePath)
        rmtree(cls.rhessysPath)
        
    def testCompareFlowtableToReference(self):
        newFlow = os.path.join('flow', 'CF10')
        prevDir = os.getcwd()
        os.chdir(self.rhessysPath)
        # Generate flow table for current version of CF
        result = self.grassdatalookup.g.read_command(self.cfBinPath, out=newFlow, \
                                                    template='templates/dr5_514_5m', \
                                                    stream='streams', road='roads', \
                                                    dem='dem', slope='slope', cellsize='5')
        self.assertTrue(result)
        
        # Compare just-generated flow table with reference CF9.1 flowtable
        referenceFlow = os.path.abspath( os.path.join('flow', 'DR5_5m_cf91.flow') )
        newFlow = os.path.abspath( "%s.flow" % (newFlow,) )
        self.assertTrue( filecmp.cmp(referenceFlow, newFlow) )
        
        os.chdir(prevDir)
