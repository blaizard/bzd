import urllib.parse


def encodeURIComponent(uri: str) -> str:
	return urllib.parse.quote_plus(uri)
