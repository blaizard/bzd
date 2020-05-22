#!/usr/bin/python3
# -*- coding: iso-8859-1 -*-

import sys
import json

if __name__== "__main__":

	output = sys.argv[1]
	print("Info script! generating {}".format(output))

	data = {
		"input_raw": "",
		"input_prepare": "",
		"size": "",
	}

	with open(output, "w+") as f:
		f.write(json.dumps(data))
