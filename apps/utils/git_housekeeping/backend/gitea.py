import json
import typing

from bzd.http.client import HttpClient


class Gitea:
	"""Gitea backend implementation."""

	def __init__(self, url: str, token: str) -> None:
		if not url:
			raise ValueError("--api-url or the GITEA_URL environment variable is required.")
		if not token:
			raise ValueError("--token or the GITEA_TOKEN environment variable is required.")
		self.url = url
		self.token = token

	def fetchOpenPullRequestBranches(self, repo: str) -> typing.Set[str]:
		"""Fetch the branch names of all open pull requests from the Gitea API.

		Args:
			repo: The repository in the 'owner/name' format.

		Returns:
			A set of branch names with open pull requests.

		Raises:
			RuntimeError: If the open pull requests could not be fetched.
		"""

		url = f"{self.url}/repos/{repo}/pulls"
		branches: typing.Set[str] = set()
		page = 1
		limit = 50

		while True:
			response = HttpClient.get(
				url,
				query={"state": "open", "limit": limit, "page": page},
				headers={"Authorization": f"token {self.token}"},
			)

			try:
				pulls = response.json
			except json.JSONDecodeError as e:
				raise RuntimeError(f"Failed to fetch open pull requests (page {page}): {e}") from e

			if not isinstance(pulls, list):
				raise RuntimeError(f"Failed to fetch open pull requests (page {page}): unexpected response.")

			for pull in pulls:
				if not isinstance(pull, dict):
					raise RuntimeError(f"Failed to fetch open pull requests (page {page}): unexpected entry.")
				head = pull.get("head")
				if not isinstance(head, dict) or not head.get("ref"):
					raise RuntimeError(f"Failed to fetch open pull requests (page {page}): missing head reference.")
				branches.add(head["ref"])

			if len(pulls) < limit:
				break
			page += 1

		return branches
