import pathlib
import datetime
import typing
import argparse
import traceback
import os
import time

from bzd.http.client import HttpClient, HttpClientException
from bzd.logging import Logger
from apps.artifacts.api.python.node.node import Node
from apps.trader_python.recording.config_polygon import apiKey

logger = Logger("polygon.io")


class Generator:
	"""Generate recording using polygon.io data."""

	def __init__(self, ticker: str, node: Node) -> None:
		self.ticker = ticker
		self.node = node

	def generate(self, dateStr: str) -> None:
		url = f"https://api.polygon.io/v2/aggs/ticker/{self.ticker}/range/1/minute/{dateStr}/{dateStr}?apiKey={apiKey}&limit=50000"

		while url:
			try:
				response = HttpClient.get(url)
				data = response.json
				url = data.get("next_url")

				# Save all the data to remote.
				with self.node.publishBulk(path=["ohlc", self.ticker, "USD", "polygon.io"],
				                           isFixedTimestamp=True) as publish:
					for item in data.get("results", []):
						publish(timestamp=item["t"],
						        data={
						            "price": item.get("vw") or (item["o"] + item["l"] + item["c"] + item["l"]) / 4,
						            "volume": item["v"],
						            "open": item["o"],
						            "close": item["c"],
						            "high": item["h"],
						            "low": item["l"],
						        })

			except HttpClientException as e:
				if e.status == 429:
					logger.info("Reached API limit, waiting 15s...")
					time.sleep(15)
				else:
					raise


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Polygon data scrapper.")
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
	node = Node(uid=args.uid)

	tickerList = ["AAPL"]

	date = datetime.date(2024, 7, 27)
	dateEnd = datetime.date(2024, 12, 31)
	dateIncrement = datetime.timedelta(days=1)

	while date <= dateEnd:
		dateStr = date.strftime("%Y-%m-%d")
		logger.info(f"Processing {len(tickerList)} ticker(s) for {dateStr}...")
		for ticker in tickerList:
			Generator(ticker, node).generate(dateStr)
		date += dateIncrement
