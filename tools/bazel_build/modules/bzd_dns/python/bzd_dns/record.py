import typing

RecordsJson = typing.Dict[str, str]


class Record:
	"""Base class for records."""

	multiple = False
	mandatory = []
	optional = []

	def __init__(self, data: RecordsJson) -> None:
		self.data = data

		checked = set()
		# Check mandatory arguments
		for attribute in self.mandatory + ["type"]:
			assert attribute in self.data, f"Missing mandatory attribute '{attribute}' in {self}."
			checked.add(attribute)
		# Check optional arguments
		for attribute in self.optional + ["ttl"]:
			checked.add(attribute)
		# Check that all attributes have been processed
		for attribute in self.data.keys():
			assert attribute in checked, f"The attribute '{attribute}' is not valid for {self}."

	@staticmethod
	def make(data) -> "Record":
		typesToClass = {
		    "a": RecordA,
		    "aaaa": RecordAAAA,
		    "cname": RecordCNAME,
		    "txt": RecordTXT,
		    "mx": RecordMX,
		}
		assert "type" in data, f"Every record must have a 'type' field: {data}."
		assert data["type"] in typesToClass, f"Unsupported record type '{data['type']}'."
		return typesToClass[data["type"]](data)

	def get(self, attribute: str, defaultValue: typing.Any = None) -> typing.Any:
		if attribute in self.data:
			return self.data[attribute]
		return defaultValue

	def merge(self, subdomain: str, zone: typing.List["Record"]) -> None:
		if not self.multiple:
			assert not any([isinstance(record, type(self)) for record in zone
			                ]), f"There can be only a single record {self} for the subdomain '{subdomain}'."
		zone.append(self)

	def visit(self, visitor: typing.Any) -> None:
		methodName = "visit" + type(self).__name__
		assert hasattr(visitor, methodName), f"The visitor is missing a method named '{methodName}'."
		getattr(visitor, methodName)(self)

	def __repr__(self) -> str:
		content = f"<{type(self).__name__} "
		content += " ".join([f"{k}=\"{v}\"" for k, v in self.data.items() if k != "type"])
		return content + ">"


class RecordA(Record):
	"""A records are for IPv4 addresses only and tell a request where your domain should direct to."""

	mandatory = ["value"]


class RecordAAAA(Record):
	"""AAAA records are for IPv6 addresses only and tell a request where your domain should direct to."""

	mandatory = ["value"]


class RecordCNAME(Record):
	"""CNAME records act as an alias by mapping a hostname to another hostname."""

	mandatory = ["value"]


class RecordTXT(Record):
	"""TXT records are used to associate a string of text with a hostname. These are primarily used for verification."""

	mandatory = ["value"]


class RecordMX(Record):
	"""MX records specify the mail servers responsible for accepting emails on behalf of your domain,
	and priority value if your provider has a number of mail servers for contingency.
	"""

	multiple = True
	mandatory = ["value", "priority"]
