import argparse

if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="Echo command.")
	parser.add_argument("messages", nargs="+", help="Time in seconds.")
	args = parser.parse_args()

	print(" ".join(args.messages))
