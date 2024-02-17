import argparse
import pathlib
import typing
import json

from bzd_dns.zones import Zones
from bzd_dns.octodns import Octodns

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="DNS provider builder.")
	parser.add_argument("--output", default=None, type=pathlib.Path, help="Output directory.")
	parser.add_argument("--config", default=None, type=pathlib.Path, help="Configuration used for the provider.")
	parser.add_argument(
	    "--input",
	    nargs=2,
	    dest="inputs",
	    metavar=("domain", "path"),
	    action="append",
	    help="Input pair of zone files and their domain.",
	)

	args = parser.parse_args()

	# Build up the domains zones.
	domains = {}
	for domain, path in args.inputs:
		dataStr = pathlib.Path(path).read_text()
		data = json.loads(dataStr)
		domains.setdefault(domain, Zones()).add(data)

	provider = Octodns(output=pathlib.Path(args.output))
	provider.process(domains)

	sys.exit(1)
