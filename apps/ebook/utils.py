import math
import typing


def sizeToString(sizeBytes: int) -> str:
	"""Converts a size in bytes to a human-readable string (e.g., KB, MB, GB)."""

	if sizeBytes == 0:
		return "0 Bytes"

	# Define the units and their corresponding powers of 1024
	units = ["Bytes", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"]

	i = int(math.floor(math.log(sizeBytes, 1024)))
	p = math.pow(1024, i)
	size = round(sizeBytes / p, 2)

	return f"{size} {units[i]}"


def percentToString(ratio: float) -> str:
	"""Convert a ratio into a percentage."""

	return f"{(ratio * 100):.1f}%"


def clusteringDimensions(dimensions: typing.List[typing.Tuple[int, int]],
                         tolerance: float) -> typing.List[typing.List[typing.Tuple[int, int]]]:
	"""Cluster dimensions using the max tolerance as deviation."""

	# Sort the items. Sorting by width primarily and then by height
	# helps to process items that are "close" in both dimensions together.
	# This is important for the greedy grouping strategy.
	sortedDimensions = sorted(dimensions, key=lambda x: (x[0], x[1]))

	clusters = []
	currentCluster: typing.Optional[typing.List[typing.Tuple[int, int]]] = None
	maxAllowedWidth: float = 0
	maxAllowedHeight: float = 0
	for width, height in sortedDimensions:

		if currentCluster is not None:
			if width <= maxAllowedWidth and height <= maxAllowedHeight:
				currentCluster.append((width, height))
			else:
				currentCluster = None

		if currentCluster is None:
			currentCluster = []
			clusters.append(currentCluster)
			currentCluster.append((width, height))
			maxAllowedWidth = width * (1 + tolerance)
			maxAllowedHeight = height * (1 + tolerance)

	return clusters


def estimateDefaultDPI(dimensions: typing.List[typing.Tuple[int, int]]) -> typing.Optional[typing.Tuple[int, int]]:
	"""Calculate the DPI based on the most common page size."""

	if len(dimensions) == 0:
		print("There are no images to process, cannot estimate DPI.")
		return None

	clusters = clusteringDimensions(dimensions, tolerance=0.05)
	clusters.sort(key=lambda x: len(x), reverse=True)
	mostCommonCluster = clusters[0]
	count = len(mostCommonCluster)
	width = int((mostCommonCluster[0][0] + mostCommonCluster[-1][0]) / 2)
	height = int((mostCommonCluster[0][1] + mostCommonCluster[-1][1]) / 2)

	if count < len(dimensions) / 2:
		print(
		    f"Warning: Most common dimension {width}x{height} only appears {count} times, which is less than half of the total images, ignoring."
		)
		return None

	pageType = {
	    "A4": (210, 297),
	    "A4 Lanscape": (297, 210),
	    "US Letter": (215.9, 279.4),
	    "US Letter Landscape": (279.4, 215.9),
	    "BD": (240, 320),
	    "BD Landscape": (320, 240),
	}
	dpis = []
	for name, (pageWidth, pageHeight) in pageType.items():
		dpis.append((abs((width / height) - (pageWidth / pageHeight)), name, width / (pageWidth / 25.4),
		             height / (pageHeight / 25.4)))
	dpis.sort(key=lambda x: x[0])
	dpiMatch = dpis[0]

	if dpiMatch[0] > 0.1:
		print(
		    f"Warning: The most common dimension {width}x{height} does not match any standard page size closely enough (error {dpiMatch[0]}), ignoring."
		)
		return None

	dpi = min(int(dpiMatch[2]), int(dpiMatch[3]))
	print(
	    f"Estimating page type '{dpiMatch[1]}' with DPI {dpi} from most common dimensions {width}x{height} ({count} times)."
	)
	return dpi, dpi
