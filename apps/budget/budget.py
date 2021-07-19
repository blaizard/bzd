from pathlib import Path
import argparse
import datetime
import re
import typing
import json

from bzd.validation.validation import Validation


def parser_date(string: str, args: typing.Any) -> datetime.datetime:
	Validation({"format": "string"}).validate(args)
	return datetime.datetime.strptime(string, args["format"])


def parse_copy(string: str, args: typing.Any) -> str:
	return string


def parse_number(string: str, args: typing.Any) -> float:
	Validation({"comma": "string"}).validate(args)
	if "comma" in args:
		string = string.replace(args["comma"], ".")
	return float(string)


def parse_category(string: str, args: typing.Any) -> typing.List[str]:
	string = string.lower()
	outputs = []
	for category, regexprMaybeList in args.items():
		for regexpr in regexprMaybeList if isinstance(regexprMaybeList, list) else [regexprMaybeList]:
			if re.search(regexpr, string):
				outputs.append(category)
	if not outputs:
		print("No match for '{}'".format(string))
		return [string]
	return outputs


def extract(data: typing.Any, config: typing.Any) -> typing.Any:
	types = {"date": parser_date, "number": parse_number, "category": parse_category, "copy": parse_copy}

	assert "columns" in config, "Missing columns key."

	output = {}
	for attr in config["columns"]:
		assert "index" in attr, "Missing index key."
		assert isinstance(attr["index"], int), "Key must be an integer."
		assert "output" in attr, "Configuration missing 'output'."
		key = attr["index"]

		if key < len(data):
			string = data[key].strip()
			assert "type" in attr, "Type is missing from config entry."
			assert attr["type"] in types, "Unsupported type '{}'.".format(attr["type"])
			try:
				parsed = types[attr["type"]](string, attr.get("args", {}))
				output[attr["output"]] = parsed
			except Exception as e:
				print("Error while parsing: '{}' with type '{}': '{}'".format(string, attr["type"], str(e)))
				return None
		else:
			print("Entry '{}' is missing from data: {}".format(key, data))

	return output


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Budget categorization generator.")
	parser.add_argument("--config", type=Path, help="Path of configuration file.")
	parser.add_argument("path", type=Path, help="Path of the bank statement file.")

	args = parser.parse_args()

	config = json.loads(Path(args.config).read_text())

	outputs = []
	for line in args.path.read_text().split("\n"):
		output = extract(data=line.split(";"), config=config)
		if output is not None:
			outputs.append(output)

	print(outputs)
