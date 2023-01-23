import sys

from pkg_resources import load_entry_point  # type: ignore

if __name__ == "__main__":
	sys.exit(load_entry_point("mkdocs", "console_scripts", "mkdocs")())
