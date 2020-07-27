#!/usr/bin/python3

import xml.etree.ElementTree as ET
import glob
import argparse

from members import Members
from render.markdown import MarkdownRender
from parser.doxygen import DoxygenParser

if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="Generate documentation from Doxygen XML output")
	parser.add_argument('--doxygen', dest="doxygen", default="docs/xml", help="Doxygen XML output directory")
	parser.add_argument("-o", "--output", dest="output", default="docs/md", help="Output of the generate documentation")

	args = parser.parse_args()

	doxygen = DoxygenParser()

	# Add all xml files to the parser
	fileList = glob.glob("{}/**/*".format(args.doxygen), recursive=True)
	for fileName in fileList:
		if fileName.lower().endswith(".xml"):
			try:
				root = ET.parse(fileName).getroot()
				doxygen.parse(root)
			except Exception as e:
				print("file: %s" % (fileName))
				raise e

	data = doxygen.getMembersData()
	members = Members(data)

	# Render to markdown
	render = MarkdownRender(args.output, members)
	render.process()
