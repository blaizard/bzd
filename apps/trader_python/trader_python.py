from apps.trader_python.exchange.virtual.exchange import Virtual
from apps.trader_python.recording.recording import Recording

if __name__ == "__main__":

	recording = Recording.fromFile("dsd")
	virtual = Virtual(recording)
