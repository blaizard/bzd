import pathlib
import typing

from private.secret.secret import Secret


def decrypt(payload: str, keyFile: typing.Optional[pathlib.Path] = None) -> str:
	"""Decrypt the given payload.

	Args:
		payload: The payload to decrypt.
		keyFile: The key file to be used.

	Returns:
		The decrypted payload.
	"""

	secret = Secret(keyFile=keyFile)
	return secret.decrypt(payload=payload)
