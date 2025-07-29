import unittest
import pathlib
import tempfile
import typing

from apps.ebook.pillow.images_converter import ImagesConverter


class TestRun(unittest.TestCase):

	def testCluster1(self) -> None:

		dimensions = [(100, 200), (121, 201), (102, 202)]
		clusters = ImagesConverter.clusteringDimensions(dimensions, tolerance=0.1)
		self.assertEqual(clusters, [
		    [(100, 200), (102, 202)],
		    [(121, 201)],
		])

	def testCluster2(self) -> None:

		dimensions = [(10, 20), (11, 21), (11, 22), (12, 20), (100, 200), (103, 205), (115, 210)]

		clusters = ImagesConverter.clusteringDimensions(dimensions, tolerance=0.1)
		self.assertEqual(clusters, [[(10, 20), (11, 21), (11, 22)], [(12, 20)], [(100, 200), (103, 205)], [(115, 210)]])

	def testCluster3(self) -> None:

		dimensions = [
		    (50, 100),
		    (51, 105),
		    (52, 110),
		    (53, 111),
		    (54, 112),
		]

		clusters = ImagesConverter.clusteringDimensions(dimensions, tolerance=0.1)
		self.assertEqual(clusters, [[(50, 100), (51, 105), (52, 110)], [(53, 111), (54, 112)]])

	def testClusterEmpty(self) -> None:

		dimensions: typing.List[typing.Tuple[int, int]] = []
		clusters = ImagesConverter.clusteringDimensions(dimensions, tolerance=0.1)
		self.assertEqual(clusters, [])


if __name__ == "__main__":
	unittest.main()
