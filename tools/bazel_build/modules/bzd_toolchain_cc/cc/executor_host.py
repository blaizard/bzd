import subprocess
import sys

if __name__ == "__main__":
	"""Default executor to run on the host machine."""

	try:
		subprocess.run(sys.argv[1:], check=True)
	except subprocess.CalledProcessError:
		sys.exit(1)
	sys.exit(0)
