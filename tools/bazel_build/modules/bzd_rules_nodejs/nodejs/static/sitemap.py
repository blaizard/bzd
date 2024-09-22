import argparse
import pathlib
import typing
import json


def getSitemap(api: typing.Mapping[str, typing.Mapping[str, typing.Any]]) -> typing.Dict[str, typing.Any]:
	maps = {}
	for path, data in api.get("routes", {}).items():
		# Filter out entries that are not to be seen.
		if data.get("redirect", False) or data.get("authentication", False):
			continue
		maps[path] = data
	return maps


def makeUrl(hostname: str, path: str) -> str:
	return hostname.rstrip("/") + "/" + path.lstrip("/")


def formatXML(api: typing.Dict[str, typing.Any], args: typing.Any) -> str:

	content = """<?xml version="1.0" encoding="UTF-8"?>
<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">
"""

	for path, data in api.items():
		content += """<url>
	<loc>{url}</loc>
</url>
""".format(url=makeUrl(args.hostname, path))

	content += """</urlset>
"""
	return content


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Sitemap generator.")
	parser.add_argument("--output", type=pathlib.Path, help="The output path for the generated file.")
	parser.add_argument("--hostname", type=str, help="The hostname.")
	parser.add_argument("--format", choices=["xml"], default="xml", type=str, help="The output format.")
	parser.add_argument("api", type=pathlib.Path, help="The API json file.")

	args = parser.parse_args()

	api = json.loads(args.api.read_text())
	data = getSitemap(api)

	if args.format == "xml":
		content = formatXML(data, args)

	else:
		assert False, f"Unsupported format '{args.format}'."

	args.output.write_text(content)
