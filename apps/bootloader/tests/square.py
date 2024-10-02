import argparse

if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="Square command.")
	parser.add_argument("value", type=int, help="Get the square of this value.")
	args = parser.parse_args()

	print(args.value * args.value, flush=True)
