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

	# Save original tty setting then set it to raw mode
	#old_tty = termios.tcgetattr(sys.stdin)
	#tty.setraw(sys.stdin.fileno())

	# open pseudo-terminal to interact with subprocess
	master, slave = pty.openpty()

	# Resize the terminal
	#tty.setraw(master)
	resize(master, 30, 1024)

	try:
		# use os.setsid() make it run in a new process group, or bash job control will not be enabled
		p = subprocess.Popen(["/bin/bash"] + args,
			cwd=cwd,
			env=env,
			bufsize=0,
			preexec_fn=os.setsid,
			stdin=slave,
			stdout=slave,
			stderr=slave,
			universal_newlines=True)

		while p.poll() is None:
			r, w, e = select.select([sys.stdin, master], [], [])
			if sys.stdin in r:
				data = os.read(sys.stdin.fileno(), 1024)
				os.write(master, data)
			elif master in r:
				data = os.read(master, 1024)
				if data:
					os.write(sys.stdout.fileno(), data)
	finally:
		# Restore tty settings back
		#termios.tcsetattr(sys.stdin, termios.TCSADRAIN, old_tty)
		os.close(slave)
		os.close(master)


if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="Pseudo terminal.")
	parser.add_argument("-c", "--cwd", dest="cwd", default=os.getcwd(), type=str, help="The current working directory.")
	parser.add_argument("-e",
		"--env",
		dest="env",
		type=lambda x: x.split("=", 2),
		action="append",
		default=[],
		help="Environment variable to be passed to the terminal.")
	parser.add_argument("-t", "--term", default="xterm-color", type=str, help="The terminal name.")
	parser.add_argument('rest', nargs=argparse.REMAINDER)
	args = parser.parse_args()

	env = {x[0]: "=".join(x[1:]) for x in args.env}
	env["TERM"] = args.term
	rest = args.rest[1:] if args.rest and args.rest[0] == "--" else args.rest
	main(cwd=pathlib.Path(args.cwd), env=env, args=rest)
