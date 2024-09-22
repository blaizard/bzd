import pathlib
import typing

import yaml

from bzd_dns.zones import Zones
from bzd_dns.record import Record


class Builder:

	def __init__(self, record: Record) -> None:
		self.record = record
		self.data: typing.Dict[str, typing.Any] = {}

	def add(self, attribute: str, yaml: typing.Optional[str] = None, formatStr: str = "{}") -> "Builder":
		if attribute in self.record.data:
			self.data[yaml if yaml else attribute] = formatStr.format(self.record.data[attribute])
		return self

	def addBuilder(self, attribute: str, builder: "Builder") -> "Builder":
		self.data[attribute] = builder.data
		return self


class Visitor:

	def __init__(self) -> None:
		self.content: typing.Dict[str, typing.Dict[str, typing.Any]] = {}

	@staticmethod
	def insertKV(data: typing.Dict[str, typing.Any], key: str, value: typing.Optional[typing.Any] = None) -> None:
		if value is not None:
			if data.get(key) is not None:
				assert value == data.get(key), f"Value for '{key}' conflicts between '{value}' and '{data.get(key)}'."
			data[key] = value

	def add(self, record: Record, value: typing.Any) -> None:
		key = record.data["type"].upper()
		data = self.content.setdefault(key, {"value": None})
		Visitor.insertKV(data, "ttl", record.get("ttl"))
		if "value" in data:
			if data["value"] is None:
				data["value"] = value
			else:
				data["values"] = [data["value"]]
				del data["value"]
		if "values" in data:
			data["values"].append(value)

	def visitRecordA(self, record: Record) -> None:
		self.add(record, record.get("value"))

	def visitRecordAAAA(self, record: Record) -> None:
		self.add(record, record.get("value"))

	def visitRecordCNAME(self, record: Record) -> None:
		self.add(record, record.get("value") + ".")

	def visitRecordTXT(self, record: Record) -> None:
		# ';' must be escaped as they correspond to comments.
		self.add(record, record.get("value").replace(";", "\\;"))

	def visitRecordMX(self, record: Record) -> None:
		value = Builder(record).add("priority", "preference").add("value", "exchange", "{}.").data
		self.add(record, value)

	def get(self) -> typing.Sequence[typing.Mapping[str, typing.Any]]:
		return [v | {"type": k} for k, v in self.content.items()]


class Octodns:

	def __init__(self, output: pathlib.Path, relative: pathlib.Path, config: typing.Dict[str, str]) -> None:
		self.output = output
		self.relative = relative
		self.config = config
		self.zones = output / "zones"
		self.zones.mkdir(parents=True, exist_ok=True)

	def process(self, domains: typing.Dict[str, Zones]) -> None:

		for domain, zones in domains.items():
			content = {}
			for subdomain, records in zones.items():
				visitor = Visitor()
				for record in records:
					record.visit(visitor)
				content[subdomain] = visitor.get()
			contentStr = yaml.dump(content)  # type: ignore
			(self.zones / f"{domain}.yaml").write_text(contentStr)

		self.makeConfig(list(domains.keys()))

	def makeConfig(self, domains: typing.Sequence[str]) -> None:

		if self.config["provider"] == "digitalocean":
			provider = {"class": "octodns_digitalocean.DigitalOceanProvider", "token": self.config['token']}
		elif self.config["provider"] == "bind9":
			provider = {
			    "class": "octodns_bind.Rfc2136Provider",
			    "host": self.config["host"],
			    "key_name": self.config["name"],
			    "key_secret": self.config["secret"],
			    "key_algorithm": self.config.get("algorithm", "hmac-sha512")
			}
		else:
			assert False, f"Unsupported provider: '{self.config['provider']}'."

		zones = {}
		for domain in domains:
			zones[f"{domain}."] = {"sources": ["config"], "targets": ["provider"]}

		content = {
		    "providers": {
		        "config": {
		            "class": "octodns.provider.yaml.YamlProvider",
		            "directory": f"{self.relative}/zones",
		            "default_ttl": 300,
		            "enforce_order": True
		        },
		        "provider": provider
		    },
		    "zones": zones
		}

		contentStr = yaml.dump(content)  # type: ignore
		(self.output / "config.yaml").write_text(contentStr)
