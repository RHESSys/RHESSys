import os
import numpy as np

ZERO = pow(10, -5)


def movingAverage(a, n=3):
    ret = np.cumsum(a, dtype=float)
    return (ret[n - 1:] - ret[:1 - n]) / n


class Settings(object):
    def __init__(self, testsites):
        self.testsites = testsites


class MassBalanceTest(object):
    
    def __init__(self, dataFilePath):
       if not os.path.isfile(dataFilePath):
           raise IOError("Path %s does not point to a file" % (dataFilePath,) )
       if not os.access(dataFilePath, os.R_OK):
           raise IOError("Cannot read data file %s" % (dataFilePath,) )
       self.dataFilePath = os.path.abspath(dataFilePath)
    
    def run(self):
        """ Run a mass balance test
        
            @return True if the test succeeded, False otherwise
        """
        return True
    
    
class WaterBalanceTest(MassBalanceTest):
    def run(self):
        """ Test if water balances
        
            @return True if water balances, that is if the maximum 3-day moving average 
            is smaller than ZERO, False otherwise
            
            @note Original R script:
            rb = read.table("../out/wmi_basin.daily", header=T)
            rb$sd = rb$sat_def-rb$rz_storage-rb$unsat_stor
            rb$et = rb$evap+rb$trans
            tmp = diff(rb$sd)
            rb$sdiff= c(0, tmp)
            tmp = diff(rb$snowpack)
            rb$snowdiff= c(0, tmp)
            tmp = diff(rb$litter_store)
            rb$litdiff= c(0, tmp)
            tmp = diff(rb$canopy_store)
            rb$candiff= c(0, tmp)
            tmp = diff(rb$detention_store)
            rb$detdiff= c(0, tmp)
            tmp = diff(rb$gw.storage)
            rb$gwdiff= c(0, tmp)
            rb$wb = rb$precip-rb$streamflow-rb$et-rb$detdiff-rb$candiff-rb$litdiff-rb$snowdiff+rb$sdiff-rb$gwdiff
            nr = length(rb)
            rb = rb[2:nr,]
            
        """
        data = np.genfromtxt(self.dataFilePath, names=True)
        sd = data['sat_def'] - data['rz_storage'] - data['unsat_stor']
        et = data['evap'] + data['trans']
        tmp = np.diff(sd)
        sdiff = np.insert(tmp, 0, 0)
        tmp = np.diff(data['snowpack'])
        snowdiff = np.insert(tmp, 0, 0)
        tmp = np.diff(data['litter_store'])
        litdiff = np.insert(tmp, 0, 0)
        tmp = np.diff(data['canopy_store'])
        candiff = np.insert(tmp, 0, 0)
        tmp = np.diff(data['detention_store'])
        detdiff = np.insert(tmp, 0, 0)
        tmp = np.diff(data['gwstorage'])
        gwdiff = np.insert(tmp, 0, 0)
        wb = data['precip'] - data['streamflow'] - et - detdiff - candiff - litdiff - snowdiff + sdiff - gwdiff
        absWb = abs(wb)
        avgWb = movingAverage(absWb, n=3)
        # Return True if water balances, that is if the maximum 3-day moving average 
        # is smaller than our ZERO value
        return max(avgWb) < ZERO