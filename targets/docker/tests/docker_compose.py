import unittest
import typing

from targets.docker.docker_compose import DockerCompose


class TestRun(unittest.TestCase):

	def testImages(self) -> None:
		compose = DockerCompose({
		    "services": {
		        "hello": {
		            "image": "hello:world"
		        },
		        "hello2": {
		            "image": "hello:world2"
		        },
		        "registry": {
		            "image": "gcr.io/registry:none"
		        },
		        "notag": {
		            "image": "notag"
		        }
		    }
		})
		self.assertEqual(compose.getImages(), {"hello": {"world", "world2"}, "registry": {"none"}, "notag": {"latest"}})


if __name__ == "__main__":
	unittest.main()
