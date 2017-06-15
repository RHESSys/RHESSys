# run_world_gen
# Will Burke 5/24/17

if (Sys.info()[4] == "Williams-MacBook-Pro-8.local" ) { #laptop paths
  setwd("~/Google Drive/UCSB/Research/rhessys/g2wtest/")
  gisbasepath = "/Applications/GRASS-6.4.app/Contents/MacOS"
  homepath = "/Users/William/Google Drive/UCSB/Research/rhessys/g2wtest"
  gisdb = "/Users/William/Google Drive/UCSB/Research/rhessys/g2wtest"
  location_name = "GRASSczo"
  mapsetname = "PERMANENT"
} else if (.Platform$OS.type == "unix") { # work mac paths
  setwd("~/Google Drive/UCSB/Research/rhessys/g2wtest")
  gisbasepath = "/Applications/GRASS-6.4.app/Contents/MacOS"
  homepath = "/Users/wburke/Google Drive/UCSB/Research/rhessys/g2wtest"
  gisdb = "/Users/wburke/Google Drive/UCSB/Research/rhessys/g2wtest"
  location_name = "GRASSczo"
  mapsetname = "PERMANENT"
} else { # PC paths
  setwd("D://Google Drive/UCSB/Research/rhessys/g2wtest/")
  gisbasepath = "D:\\Program Files\\GRASS GIS 6.4.4"
  homepath = "D:\\Google Drive\\UCSB\\Research\\rhessys\\g2wtest"
  gisdb = "D:\\Google Drive\\UCSB\\Research\\rhessys\\g2wtest"
  location_name = "GRASSczo"
  mapsetname = "PERMANENT"
}

typepars = c(gisbasepath,homepath,gisdb,location_name,mapsetname)
remove(gisbasepath,homepath,gisdb,location_name,mapsetname)

template = "remoretestingnewworldgeneration/template.1strata"
worldfile = "remoretestingnewworldgeneration/world.czo1strata"

type = "GRASS"
overwrite = "yes"

world_gen(template=template,worldfile=worldfile,type=type,typepars=typepars,overwrite=overwrite)

