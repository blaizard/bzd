import typing

from bzd_dns.record import Record, RecordsJson

ZonesJson = typing.Dict[str, typing.List[RecordsJson]]


class Zones:

	def __init__(self) -> None:
		self.data: typing.Dict[str, typing.List[Record]] = {}

	def add(self, data: ZonesJson) -> None:
		"""Convert or merge a json input into a zones structure."""

		assert isinstance(data, dict), f"The zone json zones representation must contain a valid dictionary: {data}"
		for subdomain, records in data.items():
			assert isinstance(records, list), f"Records for subdomain '{subdomain}' must be a list: {records}"
			zone = self.data.setdefault(subdomain, [])
			for record in records:
				Record.make(record).merge(subdomain, zone)

	def toJson(self) -> ZonesJson:
		"""Convert a zones structure into its json representation."""

		return {subdomain: [record.data for record in records] for subdomain, records in self.data.items()}
