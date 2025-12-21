import os
import sys
import select
import termios
import tty
import pty
import subprocess
import argparse
import pathlib
import typing
import struct
import fcntl


def resize(fd: int, rows: int, columns: int) -> None:
	winsize = struct.pack("HHHH", rows, columns, 0, 0)
	fcntl.ioctl(fd, termios.TIOCSWINSZ, winsize)


def main(cwd: pathlib.Path, env: typing.Dict[str, str], args: typing.List[str]) -> None:
	"""Spawns a command using Popen within a pseudo-terminal for real-time I/O."""

	# Create a pseudo-terminal. `fdMaster` is for our I/O, `fdSlave` is for the child.
	fdMaster, fdSlave = pty.openpty()

	# Resize the terminal
	# tty.setraw(master)
	resize(fdMaster, 30, 1024)

	try:
		proc = subprocess.Popen(
		    args,
		    stdin=fdSlave,
		    stdout=fdSlave,
		    stderr=fdSlave,
		    cwd=cwd,
		    env=env,
		    bufsize=0,
		    # use os.setsid() make it run in a new process group, or bash job control will not be enabled
		    preexec_fn=os.setsid,
		    # Working with bytes
		    text=False,
		    # This closes the fdSlave in the parent automatically
		    close_fds=True)
	finally:
		# Close the slave side of the pty in the parent process, as the child is now using it.
		os.close(fdSlave)

	# Use a non-blocking read/write loop to handle I/O.
	while proc.poll() is None:
		# Use select to wait for data from the pty master or our own stdin
		rlist, _, _ = select.select([fdMaster, sys.stdin.fileno()], [], [], 0.1)

		# Read from the child's stdout/stderr
		if fdMaster in rlist:
			try:
				output = os.read(fdMaster, 1024)
				if output:
					os.write(sys.stdout.fileno(), output)
				else:
					break  # Child process closed its output
			except (OSError, ValueError):
				break

		# Read from our stdin and write to the child's stdin
		if sys.stdin.fileno() in rlist:
			try:
				input_data = os.read(sys.stdin.fileno(), 1024)
				if input_data:
					os.write(fdMaster, input_data)
				else:
					# stdin is closed, break
					break
			except (OSError, ValueError):
				break

	# Wait for the process to fully exit
	proc.wait()
	os.close(fdMaster)  # Close the master fd when done
	sys.exit(proc.returncode)


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Pseudo terminal.")
	parser.add_argument(
	    "-c",
	    "--cwd",
	    dest="cwd",
	    default=os.getcwd(),
	    type=str,
	    help="The current working directory.",
	)
	parser.add_argument(
	    "-e",
	    "--env",
	    dest="env",
	    type=lambda x: x.split("=", 2),
	    action="append",
	    default=[],
	    help="Environment variable to be passed to the terminal.",
	)
	parser.add_argument("-t", "--term", default="xterm-color", type=str, help="The terminal name.")
	parser.add_argument("rest", nargs=argparse.REMAINDER)
	args = parser.parse_args()

	# Update the environment variables with the provided variables
	env = dict(os.environ) | {"TERM": args.term, "PYTHONUNBUFFERED": "1"}
	for x in args.env:
		env[x[0]] = "=".join(x[1:])

	rest = args.rest[1:] if args.rest and args.rest[0] == "--" else args.rest
	main(cwd=pathlib.Path(args.cwd), env=env, args=rest)
