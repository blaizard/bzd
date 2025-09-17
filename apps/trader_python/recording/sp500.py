from bzd.http.client import HttpClient
from apps.artifacts.api.python.node.node import Node

from lxml import html
import argparse
import os
import json
import typing


def getSP500Companies() -> typing.List[typing.Dict[str, str]]:
	"""Fetches the S&P 500 company list from Wikipedia."""

	url = "https://en.wikipedia.org/wiki/List_of_S%26P_500_companies"
	headers = {"User-Agent": "Mozilla/5.0"}

	response = HttpClient.get(url, headers=headers)

	# Parse the HTML content using lxml
	tree = html.fromstring(response.text)  # type: ignore

	# Use XPath to find the table and then all rows in its body
	# The XPath looks for a table with id='constituents' and then selects all
	# table rows (tr) within its table body (tbody).
	rows = tree.xpath('//table[@id="constituents"]/tbody/tr')

	companies = []
	# Skip the first row, as it's the header
	for row in rows[1:]:
		cells = row.xpath('.//td/text()')
		# Some cells might have links (<a> tags), so we refine the extraction
		# to get all text within the cell.
		data = [c.text_content().strip() for c in row.xpath('.//td')]

		if len(data) > 3:
			company = {"symbol": data[0], "name": data[1], "sector": data[2], "industry": data[3]}
			companies.append(company)

	companies.sort(key=lambda x: x["symbol"])

	return companies


if __name__ == '__main__':
	parser = argparse.ArgumentParser(description="Index S&P 500 data scrapper.")
	parser.add_argument("--node-token",
	                    type=str,
	                    default=os.environ.get("BZD_NODE_TOKEN"),
	                    help="A token to be used to access the node server.")
	parser.add_argument(
	    "uid",
	    nargs="?",
	    default=os.environ.get("BZD_NODE_UID"),
	    help=
	    "The UID of this node. If no UID is provided, the application will report the monitoring on the command line.")
	args = parser.parse_args()

	data = getSP500Companies()

	if args.uid is None:
		print(json.dumps(data, sort_keys=True, indent=4))

	else:
		node = Node(uid=args.uid)
		node.publish(data, path=["sp500"])
