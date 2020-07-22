import argparse
import os
import sys
from yapf.yapflib.yapf_api import FormatFile
from tools.sanitizer.utils.workspace import Files

configFile = os.path.join(os.path.dirname(__file__), "yapf.ini")

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
        result = FormatFile(path, style_config = configFile, in_place = True)
        assert result[1] == "utf-8", "Wrong encoding {}, must be utf-8".format(result[1])
        break

    sys.exit(0)
