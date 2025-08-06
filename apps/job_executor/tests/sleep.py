import argparse
import time

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Sleep utility.")
	parser.add_argument("timeS", type=int, help="Time to sleep in seconds.")
	args = parser.parse_args()

	print(f"Going to sleep for {args.timeS}s, counting...", flush=True)
	for elapsed in range(args.timeS):
		time.sleep(1)
		print(f"{elapsed + 1}s", flush=True)
	print("Done.", flush=True)
