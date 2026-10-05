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


def maybeDecrypt(payload: str, keyFile: typing.Optional[pathlib.Path] = None) -> str:
	"""Decrypt the given payload if it is a secret, otherwise return it unchanged.

	Args:
		payload: The payload to decrypt.
		keyFile: The key file to be used.

	Returns:
		The decrypted payload, or the payload itself if it is not a secret.
	"""

	secret = Secret(keyFile=keyFile)
	maybeSecret, _ = secret.tryReadSecret(payload=payload)
	if maybeSecret is None:
		return payload
	return secret.decrypt(payload=payload)
