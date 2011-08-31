#!/usr/bin/python
#
# compare_timings.py
#
# Copyright (C) 2011 PixFC Team (pixelfc@gmail.com)
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public  License as published by the
# Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#
import getopt
import os
import subprocess
import sys

# are the paths supplied as arguments pointing to two executables or two text files ?
text_input = False

def	get_timings_from_unit_testing(file_path):
	global text_input
	timing_dict = {}
	lines = ""

	if text_input == True:
		# the path points to a text file with the output of a unit-testing command
		lines = open(file_path).readlines()
	else:
		# run the unit test app and grab its stdout
		proc = subprocess.Popen(file_path, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
		lines = proc.communicate()[0].splitlines()

	# parse stdout and collect conversion timings in a dict
	for line in lines:
		# skip header line and blank lines
		if line.startswith("Conversion Block Name") or len(line.rstrip()) == 0:
			continue
		fields = line.split('\t')
		conversion_name = fields[0].rstrip()
		conversion_time = float(fields[1].rstrip())
		timing_dict[conversion_name] = conversion_time
		
	return timing_dict
	
def usage():
	print "Usage: ", sys.argv[0], " [-t] <path to unit-testing 1> <path to unit-testing 2>"
	print "Run the 'unit-testing' app twice and ompare the execution speeds of conversion routines."
	print "If '-t' is specified, the two paths point to a text file containing the output of 'unit-testing'."
	print "Otherwise, the two paths point to two 'unit-testing' executables."
	sys.exit(1)

def parse_args():
	global text_input

	try:
		opts, input_files = getopt.getopt(sys.argv[1:], "t")
	except getopt.GetoptError, err:
		print str(err)
		usage()

	for opt, val in opts:
		if opt == '-t':
			text_input = True
		else:
			print "Unknown option ", opt
			usage()

	if len(input_files) != 2:
		print "Need two input files (", len(input_files), "specified)"
		usage()

	if os.path.exists(input_files[0]) == False:
		print input_files[0], " does not exist"
		sys.exit(1)

	if os.path.exists(input_files[1]) == False:
		print input_files[1], " does not exist"
		sys.exit(1)

	return input_files

if __name__ == "__main__":
	# parse command line arguments
	input_files = parse_args()

	print "Running first unit test"
	timing_dict1 = get_timings_from_unit_testing(input_files[0])

	print "Running second unit test"
	timing_dict2 = get_timings_from_unit_testing(input_files[1])

	results = {}

	# go through results from second test
	for name, time in sorted(timing_dict2.iteritems()):
		if name in timing_dict1:
			results[name] = time - timing_dict1[name]
		else:
			print "Conversion '{0:60}' does not exist in first unit test".format(name)

	# go through results from first test
	for name in sorted(timing_dict1.keys()):
		if name not in timing_dict2:
			print "Conversion '{0:60}' does not exist in second unit test".format(name)

	print "Results (negative means second test was faster)"
	for name, time in sorted(results.iteritems()):
		print "{0:60}\t{1: .3f}".format(name, time)


