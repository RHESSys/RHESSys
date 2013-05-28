#!/bin/bash

# Old style lairead command
#/usr/local/bin/lairead -pre test -old world.bc -redef world.bc.Y2012M6D18H14 -allom allometric.txt

# New GRASS based lairead command
./lairead old=./world.testing redef=./world.testing.Y2005M10D2H1 allom=./allometric.txt lai=lai vegid=vegid zone=patch120 hill=hillslope patch=patch120
