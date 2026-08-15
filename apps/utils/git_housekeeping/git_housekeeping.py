import argparse
import datetime
import os
import re
import typing

from apps.utils.git_housekeeping.backend.gitea import Gitea
from bzd.logging import Logger
from bzd.utils.run import localCommand

logger = Logger("git_housekeeping")


def deleteBranch(branch: str) -> None:
	"""Delete a remote branch using the git cli.

	Args:
		branch: The name of the branch to delete.

	Raises:
		RuntimeError: If the branch deletion failed.
	"""

	result = localCommand(["git", "push", "origin", "--delete", branch], ignoreFailure=True)
	if not result.isSuccess():
		raise RuntimeError(f"Failed to delete branch '{branch}'.")


def isMergedInto(mainBranch: str, branch: str) -> bool:
	"""Check whether a branch is fully merged into the main branch.

	Args:
		mainBranch: The name of the main branch.
		branch: The name of the branch to check.

	Returns:
		True if the branch is fully merged into the main branch.
	"""

	result = localCommand(
		[
			"git",
			"merge-base",
			"--is-ancestor",
			f"refs/remotes/origin/{branch}",
			f"refs/remotes/origin/{mainBranch}",
		],
		ignoreFailure=True,
	)
	returnCode = result.getReturnCode()
	if returnCode == 0:
		return True
	if returnCode == 1:
		return False
	raise RuntimeError(f"Failed to check if branch '{branch}' is merged into '{mainBranch}'.")


def getRemoteBranches() -> typing.List[typing.Tuple[str, int]]:
	"""Read the remote branch names and their last commit timestamps.

	Returns:
		A list of (branch, commitTimestamp) tuples.
	"""

	result = localCommand(
		["git", "for-each-ref", "--format=%(refname)|%(committerdate:unix)", "refs/remotes/origin/"]
	).getStdout()
	branches: typing.List[typing.Tuple[str, int]] = []
	for line in result.splitlines():
		ref, commitTimestampStr = line.split("|", 1)
		branch = ref[len("refs/remotes/origin/") :]
		if not branch:
			continue
		branches.append((branch, int(commitTimestampStr)))
	return branches


def isProtectedBranch(branch: str, mainBranch: str) -> bool:
	"""Check whether a branch is protected and must never be deleted.

	Args:
		branch: The name of the branch.
		mainBranch: The name of the main branch.

	Returns:
		True if the branch is protected.
	"""

	return (
		branch == mainBranch or branch == "HEAD" or re.match(r"^(main|master|dev|staging|release.*)$", branch) is not None
	)


def main() -> None:
	parser = argparse.ArgumentParser(description="Delete stale branches from a git hosting service.")
	parser.add_argument("--repository", required=True, help="The repository in the 'owner/name' format.")
	parser.add_argument(
		"--backend",
		required=True,
		help="The git hosting backend to use.",
	)
	parser.add_argument(
		"--api-url",
		default=os.environ.get("GITEA_URL"),
		help="The base URL of the git hosting API.",
	)
	parser.add_argument(
		"--token",
		default=os.environ.get("GITEA_TOKEN"),
		help="The API token to authenticate with.",
	)
	parser.add_argument(
		"--days-old",
		type=int,
		default=int(os.environ.get("DAYS_OLD", 60)),
		help="The number of days after which a branch is considered stale.",
	)
	parser.add_argument(
		"--main-branch",
		default=os.environ.get("MAIN_BRANCH", "master"),
		help="The main branch to check if branches are merged into.",
	)
	parser.add_argument(
		"--dry-run",
		action="store_true",
		help="Only report what would be deleted without deleting anything.",
	)
	args = parser.parse_args()

	if args.backend == "gitea":
		backend = Gitea(url=args.api_url, token=args.token)
	else:
		raise ValueError(f"Unsupported backend '{args.backend}'.")

	# Ensure local refs for all remote branches are up to date.
	localCommand(["git", "fetch", "--all", "--prune"])

	# Fetch the branch names of all open pull requests (Git cannot see PR status).
	openPullRequestBranches = backend.fetchOpenPullRequestBranches(args.repository)
	logger.info(f"Open pull requests: {', '.join(sorted(openPullRequestBranches)) if openPullRequestBranches else 'none'}")

	cutoff = int((datetime.datetime.now() - datetime.timedelta(days=args.days_old)).timestamp())

	for branch, commitTimestamp in getRemoteBranches():
		# Skip the main branch and other protected branches.
		if isProtectedBranch(branch, args.main_branch):
			continue

		# Skip active branches using the last commit timestamp.
		if commitTimestamp >= cutoff:
			continue

		if isMergedInto(args.main_branch, branch):
			logger.info(f"Deleting stale merged branch: {branch}")
		elif branch in openPullRequestBranches:
			logger.warning(f"Skipping stale unmerged branch (has open PR): {branch}")
			continue
		else:
			logger.info(f"Deleting stale unmerged branch (no open PR): {branch}")

		if args.dry_run:
			logger.info(f"Dry run, skipping deletion of branch: {branch}")
		else:
			deleteBranch(branch)


if __name__ == "__main__":
	main()
