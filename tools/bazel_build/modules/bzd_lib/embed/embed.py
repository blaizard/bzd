import base64
import argparse
import pathlib
import mimetypes
import re


def ccIncludeEscapeData(data: bytes) -> str:
	charToEscaped = {
	    ord('\''): "'",
	    ord('"'): "\"",
	    ord('\x3f'): "?",
	    ord('\\'): "\\",
	    ord('\a'): "a",
	    ord('\b'): "b",
	    ord('\f'): "f",
	    ord('\n'): "n",
	    ord('\r'): "r",
	    ord('\t'): "t",
	    ord('\v'): "v",
	}
	output = ""
	for c in data:
		if c in charToEscaped:
			output += "\\" + charToEscaped[c]
		elif c >= 32 and c <= 126:
			output += chr(c)
		else:
			output += "\\x" + format(c, "x") + "\" \""
	return output


def sanitizedOutputName(output: pathlib.Path) -> str:
	return re.sub(r"[^a-zA-Z0-9_]+", "_", args.output.stem)


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Convert a file into an embedded string.")
	parser.add_argument("--output", default="output.txt", type=pathlib.Path, help="The output file path.")
	parser.add_argument("--format", choices=["mjs-base64", "mjs-text", "cc-include"], help="The format of the output.")
	parser.add_argument("input", type=pathlib.Path, help="The input file to embed.")

	args = parser.parse_args()

	# Guess the mime type.
	mimetypes.init()
	mimeType = mimetypes.guess_type(args.input)[0] or "text/html"

	# Create the payload.
	if args.format == "mjs-base64":
		data = args.input.read_bytes()
		data64 = base64.b64encode(data)
		args.output.write_text(f"export default \"data:{mimeType};base64, {data64.decode()}\";\n")

	elif args.format == "mjs-text":
		data = args.input.read_text()
		escapedData = data.replace("\"", "\\\"").replace("\n", "\\n")
		args.output.write_text(f"export default \"{escapedData}\";\n")

	elif args.format == "cc-include":
		data = args.input.read_bytes()
		escapedData = ccIncludeEscapeData(data)
		args.output.write_text(
		    f"#pragma once\nstatic constexpr const char {sanitizedOutputName(args.output)}[] = \"{escapedData}\";\n")
