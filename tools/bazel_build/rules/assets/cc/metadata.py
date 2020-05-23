#!/usr/bin/python3
# -*- coding: iso-8859-1 -*-

import sys
import json
import os

if __name__== "__main__":

	inputPath = sys.argv[1]
	inputFinalPath = sys.argv[2]
	output = sys.argv[3]

	data = {
		"size": os.path.getsize(inputFinalPath),
	}

	with open(output, "w+") as f:
		f.write(json.dumps(data))
