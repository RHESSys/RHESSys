#!/usr/bin/python

import os
import sys
import calendar 

INCHES_PER_METER = 39.3700787

output_root = sys.argv[1]
ncdc_filename = sys.argv[2]

infile = open(ncdc_filename, "r")

# Switch these back to *.temp when done debugging
tmaxfile = open(''.join([output_root, ".tmax"]), "w")
tminfile = open(''.join([output_root, ".tmin"]), "w")
precipfile = open(''.join([output_root, ".rain"]), "w")

# Use the first line to determine if data was downloaded with data flags.
# Currently ignores those flags. If this is a problem for you, then we
# should fix it.
line = infile.readline()
fields = line.split(',')

day_step = 2
if (fields[9] == 'F'):
	day_step = 4

# Second line is trash
infile.readline()

# Flag to say this is the first line and add the date header
# to the top of the climate output files
header = 1

for line in infile:
	fields = line.split(',')

	data_type = fields[4]

	yearmonth = fields[6]
	year = yearmonth[0:4]
	month = yearmonth[4:6]
	(first_day, days) = calendar.monthrange(int(year), int(month)) 

	if header == 1:
		header = 0
		precipfile.write(''.join([str(year), ' ', str(month), ' 1 1\n']))
		tmaxfile.write(''.join([str(year), ' ', str(month), ' 1 1\n']))
		tminfile.write(''.join([str(year), ' ', str(month), ' 1 1\n']))

	for current_day in range(1, days+1):
		# Get the array index for this day
		index = 8 + day_step * (current_day - 1)
		if data_type == 'PRCP':
			# convert from hundredths of inches to meters
			precip = fields[index]
			if precip.strip() == '-99999' or precip.strip() == '99999':
				# Currently replacing NULL as zero for a
				# first approximation
				precipfile.write('0\n')
			else:
				precip = (int(precip)/100.0) / INCHES_PER_METER 
				precipfile.write(''.join([str(precip).strip(), '\n']))
		elif data_type == 'TMAX':
			# convert from F to C
			temp = fields[index]
			if temp.strip() == '-99999' or temp.strip() == '99999':
				tmaxfile.write('-99999\n')
			else:
				temp = (5.0/9)*(int(fields[index]) - 32)
				tmaxfile.write(''.join([str(temp).strip(), '\n']))
		elif data_type == 'TMIN':
			# convert from F to C
			temp = fields[index]
			if temp.strip() == '-99999' or temp.strip() == '99999':
				tminfile.write('-99999\n')
			else:
				temp = (5.0/9)*(int(fields[index]) - 32)
				tminfile.write(''.join([str(temp).strip(), '\n']))

precipfile.close()
tmaxfile.close()
tminfile.close()

# Fix NODATA values of -99999 or 99999
#tmaxfile = open(''.join([output_root, ".tmax.temp"]), "r")
#tminfile = open(''.join([output_root, ".tmin.temp"]), "r")
#precipfile = open(''.join([output_root, ".rain.temp"]), "r")
#
#tmaxout = open(''.join([output_root, ".tmax"]), "w")
#tminout = open(''.join([output_root, ".tmin"]), "w")
#precipout = open(''.join([output_root, ".rain"]), "w")
#
## Copy the initial dates into the new output files
#tmaxout.write(tmaxfile.readline())
#tminout.write(tminfile.readline())
#precipout.write(precipfile.readline())
#
## For precip, replace null data with 0
## Currently this is already handled in the initial read of the NCDC file
#for line in precipfile:
#	if line.strip() == '-99999' or line.strip() == '99999':
#		precipout.write('0\n')
#	else:
#		precipout.write(line)	
#
###########
### WARNING
### Temperature fixing breaks if the very first value is NODATA
###########
#
## For tmax, record the first and last temperatures, average
## them, and set all in missing range to those values
#nodata = 0
#nodata_length = 0
#for line in tmaxfile:
#	if line.strip() == '-99999':
#		nodata = 1
#		nodata_length = nodata_length + 1
#	else:
#		if nodata == 1:
#			nodata = 0
#			# Compute the average
#			filler_temp = (float(prev) + float(line)) / 2.0
#			for i in range(1, nodata_length + 1):
#				tmaxout.write(''.join([str(filler_temp), '\n']))
#			nodata_length = 0
#		prev = line.rstrip()
#		tmaxout.write(line)
#
## For tmin, record the first and last temperatures, average
## them, and set all in missing range to those values
#noata = 0
#nodata_length = 0
#for line in tminfile:
#	if line.strip() == '-99999':
#		nodata = 1	
#		nodata_length = nodata_length + 1
#	else:
#		if nodata == 1:
#			nodata = 0
#			# Compute the average
#			filler_temp = (float(prev) + float(line)) / 2.0
#			for i in range(1, nodata_length + 1):
#				tminout.write(''.join([str(filler_temp), '\n']))
#			nodata_length = 0
#		prev = line.rstrip()
#		tminout.write(line)
#	
## Remove the temp files
#os.system(''.join(["rm ", output_root, ".tmax.temp"]))
#os.system(''.join(["rm ", output_root, ".tmin.temp"]))
#os.system(''.join(["rm ", output_root, ".rain.temp"]))
