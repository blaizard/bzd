#!/usr/bin/python3
# -*- coding: iso-8859-1 -*-

import json

def cpp(data, output):
	with open(output, "w") as f:
		f.write("#include <iostream>\nint main() { std::cout << \"Hello Wolrd!\" << std::endl; }")
