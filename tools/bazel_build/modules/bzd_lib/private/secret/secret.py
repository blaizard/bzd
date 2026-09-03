import argparse
import pathlib
import typing
import json
import zlib
import sys
import os
import subprocess

from bzd.utils.runfiles import pathFromRLocation

from private.secret.config import age as configAge, recipients as configRecipients


class Secret:
	def __init__(
		self,
		recipients: typing.Optional[typing.Union[typing.List[str], pathlib.Path]] = None,
		keyFile: typing.Optional[pathlib.Path] = None,
	) -> None:
		self.recipients = recipients if isinstance(recipients, list) else Secret.loadRecipientsFromFile(recipients)
		self.recipientsHash = Secret.recipientsToHash(self.recipients)
		self.age = pathFromRLocation(configAge)
		self.keyFile = keyFile

		assert self.age, "The path of the age tool is missing."

	@staticmethod
	def version() -> int:
		"""The current version of the tool."""

		return 1

	@staticmethod
	def loadRecipientsFromFile(recipients: typing.Optional[pathlib.Path]) -> typing.List[str]:
		"""Load a list of recipients from the given or default file."""

		if recipients is None:
			recipients = pathFromRLocation(configRecipients)
		return sorted(json.loads(recipients.read_text()).values())

	@staticmethod
	def recipientsToHash(recipients: typing.Sequence[str]) -> str:
		"""Create a non-cyrptographic hash over the recipients."""

		crc = 0
		for key in recipients:
			crc = zlib.crc32(key.encode("utf-8"), crc)
		return str(crc)

	def encrypt(self, payload: str) -> str:
		"""Encrypt the given payload."""

		# Check if the payload is already a secret.
		maybeSecret, maybeError = self.tryReadSecret(payload=payload)
		if maybeSecret is not None:
			if maybeError is None:
				# Nothing to do, the secret is properly encrypted already.
				return payload
			# The secret is not encrypted according to the environment, need to re-encrypt it.
			payload = self._decrypt(maybeSecret)

		args = [self.age.as_posix(), "--armor"]
		for recipient in self.recipients:
			args += ["--recipient", recipient]
		result = subprocess.run(
			args,
			input=payload.encode(),
			capture_output=True,
			check=True,
		)
		return json.dumps(
			{
				"version": Secret.version(),
				"recipients": self.recipientsHash,
				"secret": result.stdout.decode(),
			}
		)

	def decrypt(self, payload: str) -> str:
		"""Decrypt a payload."""

		maybeSecret, maybeError = self.tryReadSecret(payload=payload)
		assert maybeSecret is not None, f"Cannot read payload: {maybeError}"
		return self._decrypt(maybeSecret)

	def _decrypt(self, secret: str) -> str:
		"""Decrypt a secret."""

		keyFile = self.getKeyFile()
		result = subprocess.run(
			[self.age, "--decrypt", "--identity", keyFile.as_posix()],
			input=secret.encode(),
			capture_output=True,
			check=True,
		)
		return result.stdout.decode()

	def getKeyFile(self) -> pathlib.Path:
		"""Retrieve the current key file from the system or raise an exception."""

		if self.keyFile is not None:
			return self.keyFile

		maybeKeyFile = os.environ.get("BZD_KEY_FILE")
		if maybeKeyFile:
			keyFile = pathlib.Path(maybeKeyFile)
			assert keyFile.is_file(), f"The key file pointed by BZD_KEY_FILE environment variable (={keyFile}), does not exists."
			return keyFile

		maybeKeyFileFromHome = pathlib.Path.home() / ".bzd" / "key.txt"
		if maybeKeyFileFromHome.is_file():
			return maybeKeyFileFromHome

		raise Exception(
			f"No key file found, searched in:\n - Environment variable 'BZD_KEY_FILE'.\n - Path at '{maybeKeyFileFromHome}'."
		)

	def tryReadSecret(self, payload: str) -> typing.Tuple[typing.Optional[str], typing.Optional[str]]:
		"""Read the secret from the payload.

		If the payload is not correctly formatted: return None + error string.
		If the payload is readable but the metadata doesn't match the current environment, return secret + warning string.
		If the payload is readable and the metadata matches the current environment, return secret + None
		"""

		try:
			data = json.loads(payload)
			assert isinstance(data, dict), "payload must be a dictionary"
			assert "secret" in data, "missing secret field"
			assert isinstance(data["secret"], str), "secret must be a string"
			secret = data["secret"]
		except json.JSONDecodeError:
			return None, "invalid secret (decode)"
		except AssertionError as e:
			return None, f"invalid secret ({str(e)})"

		try:
			assert "version" in data, "missing version"
			assert "recipients" in data, "missing recipients"
			assert len(data.keys()) == 3, "too many keys"
			assert data["version"] == Secret.version(), f"wrong version '{data['version']}'"
			assert data["recipients"] == self.recipientsHash, "wrong recipients"
		except AssertionError as e:
			return secret, f"not up to date ({str(e)})"

		return secret, None


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Secret utility")

	groupRecipient = parser.add_mutually_exclusive_group()
	groupRecipient.add_argument(
		"--recipients", type=pathlib.Path, help="The path of the file containing the recipients public keys."
	)
	groupRecipient.add_argument("--recipient", type=str, action="append", help="Public to be used as recipient.")

	groupInput = parser.add_mutually_exclusive_group(required=True)
	groupInput.add_argument("--file", type=pathlib.Path, help="The path of the input file containing the secret/payload.")
	groupInput.add_argument("--payload", nargs=2, metavar=("KEY", "PAYLOAD"), help="The key and the payload.")

	parser.add_argument(
		"--output", type=pathlib.Path, help="If set, writes the output to this path, otherwise write to stdout."
	)
	parser.add_argument("command", choices=["check", "decrypt", "encrypt"], help="The command to be used.")

	args = parser.parse_args()

	secret = Secret(recipients=args.recipients or args.recipient)
	payload = args.file.read_text() if args.file else args.payload[1]

	output = ""
	if args.command == "check":
		_, maybeError = secret.tryReadSecret(payload=payload)
		if maybeError:
			if args.file:
				print(f"{args.file}: {maybeError}")
			else:
				print(f"{args.payload[0]}: {maybeError}")
			sys.exit(1)

	elif args.command == "decrypt":
		output = secret.decrypt(payload=payload)

	elif args.command == "encrypt":
		output = secret.encrypt(payload=payload)

	if args.output:
		args.output.write_text(output)
	else:
		print(output)
