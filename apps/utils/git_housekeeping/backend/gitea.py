import json
import typing

from bzd.logging import Logger
from bzd.utils.run import localCommand

logger = Logger("git_housekeeping.backend.gitea")


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
		"""

		url = f"{self.url}/api/v1/repos/{repo}/pulls"
		branches: typing.Set[str] = set()
		page = 0
		limit = 50

		while True:
			result = localCommand(
				[
					"curl",
					"-s",
					"-H",
					f"Authorization: token {self.token}",
					f"{url}?state=open&limit={limit}&page={page}",
				],
				ignoreFailure=True,
			)
			if not result.isSuccess():
				logger.warning(f"Failed to fetch open pull requests (page {page}).")
				break

			try:
				pulls = json.loads(result.getStdout())
			except json.JSONDecodeError as e:
				logger.warning(f"Failed to fetch open pull requests (page {page}): {e}")
				break

			if not isinstance(pulls, list):
				break

			for pull in pulls:
				head = pull.get("head")
				if isinstance(head, dict) and head.get("ref"):
					branches.add(head["ref"])

			if len(pulls) < limit:
				break
			page += 1

		return branches
