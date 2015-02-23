from unittest import TestCase
import os, sys
from zipfile import ZipFile
from shutil import rmtree
import subprocess, shlex

import settings

import rhessystest

class TestWaterBalance(TestCase):
    
    @classmethod
    def setUpClass(cls):
        rhessysBin = os.path.join( './', os.environ['RHESSYS_BIN'] )
        cls.rhessys = os.path.abspath(rhessysBin)
        cls.settings = settings.settings
        cls.dataRoot = os.path.abspath('./test/data')
        cls.testRoot = os.path.join(cls.dataRoot, 'testtmp')
        cls.sites = []
        # Make a place to unzip test datasets to
        if os.path.exists(cls.testRoot):
            rmtree(cls.testRoot)
        else:
            os.mkdir(cls.testRoot)
        
        for testsite in cls.settings.testsites:
            # Unpack test site model from zipfile
            zipFile = "%s.zip" % (testsite['name'],)
            zipPath = os.path.join(cls.dataRoot, zipFile)
            if not os.access(zipPath, os.R_OK):
                raise IOError(errno.EACCES, "Unable to read test data zip %s" %
                      zipPath)
            zip = ZipFile(zipPath, 'r')
            zip.extractall(path=cls.testRoot)
            testsite['testpath'] = os.path.join(cls.testRoot, testsite['name'])
            # Run RHESSys
            runDir = os.path.join(testsite['testpath'], 'scripts')
            cmdline = cls.rhessys + ' ' + testsite['cmdline']
            args = shlex.split(cmdline)
            p = subprocess.Popen(args, cwd=runDir, 
                                 stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            (stdout, stderr) = p.communicate()
            result = p.returncode
            if result != 0:
                sys.stderr.write(stdout)
                sys.stderr.write(stderr)
                raise Exception("Failed to run RHESSys for test site %s, command: %s, cwd: %s" % \
                                (testsite['name'], cmdline, runDir) )
            # Record path to output file path
            outfileName = "testcase_{spatial_agg}.{temporal_agg}"
            outfileName = outfileName.format(spatial_agg=testsite['spatial_agg'],
                                             temporal_agg=testsite['temporal_agg'])
            testsite['outputpath'] = os.path.join(testsite['testpath'], 'out', outfileName)
            
    
    @classmethod
    def tearDownClass(cls):
        rmtree(cls.testRoot)
    
    def testWaterBalance(self):
        for testsite in self.settings.testsites:
            wbTest = rhessystest.WaterBalanceTest(testsite['outputpath'])
            self.assertTrue( wbTest.run() )
    
    def testCarbonBalance(self):
        pass
    
    def testNitrogenBalance(self):
        pass
    
    def testEnergyBalance(self):
        pass