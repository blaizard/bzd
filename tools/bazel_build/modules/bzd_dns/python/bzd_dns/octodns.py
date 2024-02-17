import pathlib
import typing

from bzd_dns.zones import Zones
from bzd_dns.record import Record


class Visitor:

	def __init__(self) -> None:
		self.content = ""

	def setLine(self, record: Record, attribute: str, yaml: typing.Optional[str] = None) -> None:
		if attribute in record.data:
			self.content += f"    {yaml if yaml else attribute}: {record.data[attribute]}\n"

	def setCommon(self, record: Record) -> None:
		self.content += f"  - type: {record.data['type'].upper()}\n"
		self.setLine(record, "ttl")

	def visitRecordA(self, record: Record) -> None:
		self.setCommon(record)
		self.setLine(record, "value")

	def visitRecordAAAA(self, record: Record) -> None:
		self.setCommon(record)
		self.setLine(record, "value")


class Octodns:

	def __init__(self, output: pathlib.Path) -> None:
		self.output = output
		self.zones = output / "zones"
		self.zones.mkdir(parents=True, exist_ok=True)

	def process(self, domains: typing.Dict[str, Zones]) -> None:

		for domain, zones in domains.items():
			content = "---\n"
			for subdomain, records in zones.items():
				content += f"'{subdomain}':\n"
				for record in records:
					visitor = Visitor()
					record.visit(visitor)
					content += visitor.content
			(self.zones / f"{domain}.yaml").write_text(content)

		self.makeConfig(domains.keys())

	def makeConfig(self, domains: typing.Sequence[str]) -> None:

		content = "---\n"
		content += "providers:\n"
		content += "  config:\n"
		content += "    class: octodns.provider.yaml.YamlProvider\n"
		content += f"    directory: {self.zones}\n"
		content += "    default_ttl: 300\n"
		content += "    enforce_order: true\n"
		content += "  provider:\n"
		content += "    class: octodns_digitalocean.DigitalOceanProvider\n"
		content += "    token: abc\n"
		content += "zones:\n"
		for domain in domains:
			content += f"  {domain}.:\n"
			content += "    sources:\n"
			content += "      - config\n"
			content += "    targets:\n"
			content += "      - provider\n"

		(self.output / "config.yaml").write_text(content)
