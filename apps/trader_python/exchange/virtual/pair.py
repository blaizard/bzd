from apps.trader_python.recording.recording import RecordingPair


class VirtualPair:

	def __init__(self, pair: RecordingPair) -> None:
		self.pair = pair
