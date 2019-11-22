#!/usr/bin/python3
# -*- coding: iso-8859-1 -*-

import xml.etree.ElementTree as ET
import glob
import argparse

from render import Render
from members import Members
from parser.doxygen import DoxygenParser

if __name__== "__main__":

	parser = argparse.ArgumentParser(description="Generate documentation from Doxygen XML output")
	parser.add_argument('--doxygen', dest="doxygen", default="docs/xml", help="Doxygen XML output directory")
	parser.add_argument("-o", "--output", dest="output", default="docs/md", help="Output of the generate documentation")

	args = parser.parse_args()

	parser = DoxygenParser()

	# Add all xml files to the parser
	fileList = glob.glob("{}/**/*".format(args.doxygen), recursive=True)
	for fileName in fileList:
		if fileName.lower().endswith(".xml"):
			try:
				root = ET.parse(fileName).getroot()
				parser.parse(root)
			except Exception as e:
				print("file: %s" % (fileName))
				raise e

	members = Members(parser.getMembersData())

	# Render to markdown
	render = Render(args.output)
	render.process(members)
