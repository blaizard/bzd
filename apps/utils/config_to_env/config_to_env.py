import argparse
import pathlib
import os
import typing

from apps.utils.config_to_env.config_python import secrets


class GithubEnvironment:
	def __init__(self) -> None:
		assert "GITHUB_OUTPUT" in os.environ, "'GITHUB_OUTPUT' environment variable is missing."
		self.output = pathlib.Path(os.environ["GITHUB_OUTPUT"])
		assert self.output.is_file(), "'GITHUB_OUTPUT' environment variable does not point to a valid file."

	def _load(self, environmentVariables: typing.Dict[str, str]) -> None:
		with self.output.open("a", encoding="utf-8") as f:
			for key, value in environmentVariables.items():
				# Use heredoc delimiter syntax for multiline values (e.g. SSH keys)
				if "\n" in value:
					delimiter = "EOF_DELIMITER"
					f.write(f"{key}<<{delimiter}\n{value}\n{delimiter}\n")
				else:
					f.write(f"{key}={value}\n")

	def loadSecrets(self, secrets: typing.Dict[str, str]) -> None:
		for key, value in secrets.items():
			for line in value.splitlines():
				if line.strip():
					print(f"::add-mask::{line}")
		self._load(secrets)


GiteaEnvironment = GithubEnvironment

environments = {"gitea": GiteaEnvironment, "github": GithubEnvironment}

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Load the given configuration into the environment.")
	parser.add_argument(
		"--environment", required=True, choices=environments.keys(), help="The environment to load the variables into."
	)
	args = parser.parse_args()

	environment = environments[args.environment]()
	environment.loadSecrets({k: str(v) for k, v in secrets().items()})
