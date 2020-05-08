
import os
import sys
import tempfile
import importlib
from collections import namedtuple

GRASSConfig = namedtuple('GRASSConfig', ['gisbase', 'dbase', 'location', 'mapset'], verbose=False)

class GrassDataLookup(object): 
    def __init__(self, grass_scripting=None, grass_lib=None, grass_config=None):
        """ @brief Constructor for GrassDataLookup
        
            @param grass_scripting Previously imported grass.script (GRASS scripting API), 
            if None, grass.script will be imported
            @param grass_lib Previously imported grass.lib.gis (low-level GRASS API); if None
            grass.lib.gis will be imported
            @param grass_config GRASSConfig instance 
        """
        self.grass_config = grass_config
        
        if not grass_scripting:
            self.g = self._setupGrassScriptingEnvironment()
        else:
            self.g = grass_scripting
            
        if not grass_lib:
            self.grass_lowlevel = self._setupGrassEnvironment()

        else:
            self.grass_lowlevel = grass_lib 

    def _setupGrassScriptingEnvironment(self):
        """ @brief Set up GRASS environment for using GRASS scripting API from 
            Python (e.g. grass.script)
        """
        os.environ['GISBASE'] = self.grass_config.gisbase
        sys.path.append(os.path.join(self.grass_config.gisbase, 'etc', 'python'))
        import grass.script.setup as gsetup
        gsetup.init(self.grass_config.gisbase, \
                    self.grass_config.dbase, self.grass_config.location, \
                    self.grass_config.mapset)

        self.g = importlib.import_module('grass.script')
        return self.g
    
    def _setupGrassEnvironment(self):
        """ @brief Set up GRASS environment for using GRASS low-level API from 
            Python (e.g. grass.lib)
        """
        gisBase = self.grass_config.gisbase
        sys.path.append(os.path.join(gisBase, 'etc', 'python'))
        os.environ['LD_LIBRARY_PATH'] = os.path.join(gisBase, 'lib')
        os.environ['DYLD_LIBRARY_PATH'] = os.path.join(gisBase, 'lib')
        # Write grassrc
        os.environ['GISRC'] = self._initializeGrassrc()
        os.environ['GIS_LOCK'] = str(os.getpid())
        self.grass_lowlevel = importlib.import_module('grass.lib.gis')
        self.grass_lowlevel.G_gisinit('')
        return self.grass_lowlevel
    
    def _initializeGrassrc(self):
        grassRcFile = tempfile.NamedTemporaryFile(prefix='grassrc-', delete=False)
        grassRcContent = "GISDBASE: %s\nLOCATION_NAME: %s\nMAPSET: %s\n" % \
            (self.grass_config.dbase, self.grass_config.location, self.grass_config.mapset)
        grassRcFile.write(grassRcContent)
        return grassRcFile.name