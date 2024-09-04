import time
import argparse

if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="Sleep command.")
	parser.add_argument("timeS", type=int, help="Time in seconds.")
	args = parser.parse_args()

	time.sleep(args.timeS)
