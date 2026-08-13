import argparse
import pathlib

from sandbox import SandboxContainer, Feature


class FeatureLinux(Feature):
	def process(self, context: SandboxContainer) -> None:
		self.dockerFile += [
			"RUN sudo apt-get install -y"
			+ " jq"  # For workflow
			+ " openssh-client",  # For ssh-keyscan
		]
		self.dockerFile += [
			# Add known hosts for git operations
			"RUN mkdir -p ~/.ssh",
			"RUN ssh-keyscan github.com >> ~/.ssh/known_hosts",
			"RUN ssh-keyscan -p 6222 10.10.0.2 >> ~/.ssh/known_hosts",
			# For nix-cache
			"RUN ssh-keyscan 10.10.0.15 >> ~/.ssh/known_hosts",
			"RUN chmod 644 ~/.ssh/known_hosts",
		]
		self.dockerFile += [
			# Install Nix in single-user mode (from https://github.com/NixOS/nix-installer)
			'RUN curl -sSfL https://artifacts.nixos.org/nix-installer | sh -s -- install linux --extra-conf "sandbox = false" --enable-flakes --init none --no-confirm',
			"RUN sudo chown -R ${user}:${group} /nix",
			'ENV PATH="${PATH}:/nix/var/nix/profiles/default/bin"',
		]


class FeatureOpencode(Feature):
	def process(self, context: SandboxContainer) -> None:
		self.dockerFile += [
			# This ensure the migration of the database.
			"RUN opencode db path",
			# Setup git.
			"RUN git config --global user.name 'agent'",
			"RUN git config --global user.email 'agent@blaizard.com'",
		]


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Opencode runner Dockerfile generator.")
	parser.add_argument("--output", required=True, type=pathlib.Path, help="The output path.")
	parser.add_argument("image", choices=["linux", "opencode"], help="The image to generate.")
	args = parser.parse_args()

	commonArgs = [
		"--dry",
		"--no-tty",
		"--no-user-namespace-remapping",
		"--user=testuser",
		"--home=/home/testuser",
		"--uid=1000",
		"--gids=1000",
		"--enable=isolation",
		"--isolate",
	]

	if args.image == "linux":
		sandbox = SandboxContainer.fromCLI(commonArgs, additionalFeatures=[FeatureLinux])

	elif args.image == "opencode":
		sandbox = SandboxContainer.fromCLI(
			commonArgs
			+ [
				"--enable=opencode",
			],
			additionalFeatures=[FeatureLinux, FeatureOpencode],
		)

	else:
		raise Exception(f"Unsupported image type '{args.image}'.")

	args.output.write_text(sandbox.dockerFile)
