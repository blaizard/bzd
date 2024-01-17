import base64
import argparse
import pathlib
import mimetypes

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Convert a file into an embeded string.")
	parser.add_argument("--output", default="output.txt", type=pathlib.Path, help="The output file path.")
	parser.add_argument("--format", choices=["mjs-base64", "mjs-text"], help="The format of the output.")
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
