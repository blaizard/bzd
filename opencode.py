#!/usr/bin/env -S python3 -B
import argparse
import pathlib
import typing

from sandbox import Feature, SandboxContainer


class FeatureOpenCode(Feature):
	"""Feature: Add opencode CLI to the container."""

	@staticmethod
	def cli() -> dict[str, dict[str, typing.Any]]:
		return {
			"opencode": {
				"action": "append",
				"default": [],
				"choices": ["host-config"],
				"help": "Options for the opencode feature.",
			}
		}

	def process(self, context: SandboxContainer) -> None:
		# Add Linux essential tools
		llmEssentials = [
			"jq",
			"ripgrep",
			"coreutils",
			"diffutils",
			"lsof",
			"net-tools",
			"netcat-openbsd",
			"iproute2",
			"tree",
			"coreutils",
			"findutils",
		]
		self.dockerFile += [
			f"RUN sudo apt install -y nodejs npm {' '.join(llmEssentials)}",
			"RUN sudo npm install -g @opencode/cli",
		]

		if "host-config" in self.args.opencode:
			self.dockerFile += [
				# Important! If not set, docker will create synthetic directory and write access will not be permitted.
				f"RUN mkdir -p {context.home}/.config {context.home}/.local/share {context.home}/.local/state",
			]
			hostHome = pathlib.Path.home()
			self.volumes += [
				f"{hostHome / path}:{context.home / path}"
				for path in [
					".config/opencode",
					".local/share/opencode",
					".local/state/opencode",
				]
				if (hostHome / path).exists()
			]
		else:
			# Add generic agents/commands/skills.
			self.includes(".opencode/agents", f"{context.home}/.config/opencode/agents", context)
			self.includes(".opencode/commands", f"{context.home}/.config/opencode/commands", context)
			for skill in ["cc", "debug", "documentation", "sanitize", "software-architecture", "write-tests"]:
				self.includes(f".opencode/skills/{skill}", f"{context.home}/.config/opencode/skills/{skill}", context)


class FeatureKiCad(Feature):
	"""Feature: Add KiCad to the container."""

	def process(self, context: SandboxContainer) -> None:
		self.includes(".opencode/skills/kicad", f"{context.home}/.config/opencode/skills/kicad", context)
		self.dockerFile += [
			"RUN sudo add-apt-repository -y ppa:kicad/kicad-10.0-releases",
			"RUN sudo apt-get update",
			"RUN sudo apt install -y --no-install-recommends kicad kicad-symbols kicad-footprints",
		]


class FeaturePlaywright(Feature):
	"""Feature: Add Playwright to the container."""

	def process(self, context: SandboxContainer) -> None:
		self.includes(".opencode/skills/playwright-cli", f"{context.home}/.config/opencode/skills/playwright-cli", context)
		self.dockerFile += [
			# Newer version require node v20+ (which conflicts with ubuntu stock node version).
			"RUN sudo npm install -g @playwright/cli@v0.1.15",
			"RUN playwright-cli install-browser chrome --with-deps",
		]


if __name__ == "__main__":
	parser = argparse.ArgumentParser(
		formatter_class=argparse.ArgumentDefaultsHelpFormatter,
		description="Opencode.",
	)
	parser.add_argument("--id", type=int, default=1, help="The agent identifier (0-9).")
	parser.add_argument("--build", action="store_true", help="Re-build/clean the container if set.")
	parser.add_argument(
		"--build-force",
		action="store_true",
		help="Force re-build/clean the container if set.",
	)
	parser.add_argument("--no-isolate", action="store_true", help="Disable isolation.")

	args, remaining = parser.parse_known_args()
	sandboxArgs = []
	if args.build:
		sandboxArgs += ["--build"]
	if args.build_force:
		sandboxArgs += ["--build-force"]
	if args.no_isolate:
		sandboxArgs += ["--no-isolate"]
	SandboxContainer.fromCLI(
		[
			"--opencode=host-config",
			"--enable=session",
			"--enable=isolation",
			"--enable=volume",
			"--isolate",
			f"--prefix=opencode{args.id}",
			*sandboxArgs,
			"opencode",
			*remaining,
		],
		additionalFeatures=[
			FeatureOpenCode,
			FeatureKiCad,
			FeaturePlaywright,
		],
	)
