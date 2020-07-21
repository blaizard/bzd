import argparse
import os
import sys
from autopep8 import main
from tools.sanitizer.utils.workspace import Files

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Wrapper for mypy")
    parser.add_argument("workspace", help="Workspace to be processed.")

    args = parser.parse_args()

    files = Files(args.workspace, include=[
        "**/*.py",
    ], exclude=[
        "**tools/bzd/generator/yaml**",
    ])

    for path in files.data():
        main(argv=[__file__, "--in-place", path])

    sys.exit(0)
