import typing


def getMatchingWeight(word: str, compareWith: str) -> float:
	"""Calculate the matching weight of a word into another.

	It performs a fuzzy search on the word using a fast algorithm.
	The word can ba a subset of the word to compare.
	"""

	if len(word) == 0 or len(compareWith) == 0:
		return 0

	firstChar = word[0]
	start = -1
	weight: float = 0.

	def compareWithGetCharAtOrNone(index: int) -> typing.Optional[str]:
		return compareWith[index] if index < len(compareWith) else None

	while True:
		start = compareWith.find(firstChar, start + 1)
		if start == -1:
			break
		# If this is the first character of a word, add a weight to prioritize this word
		curWeight = 1. if start == 0 else 0.9
		iSentence = start
		# Check if the word is +/- 1 character far away
		for i in range(1, len(word)):
			c = word[i]
			if compareWithGetCharAtOrNone(iSentence + 1) == c:
				curWeight += 1.
				iSentence += 1
			elif compareWithGetCharAtOrNone(iSentence + 2) == c:
				curWeight += 0.5
				iSentence += 2
			elif compareWithGetCharAtOrNone(iSentence - 1) == c:
				curWeight += 0.5
			else:
				iSentence += 1

		weight = max(weight, curWeight)

	# Normalize the result and make the result exponential to add importance on full matches
	weight /= len(word)

	# Add a penalty on word size to ensure that an exact match always get more weight than a substring match
	weight = weight if len(word) == len(compareWith) else (weight * 0.9)

	return weight


def sortMatchingWeight(word: str, compareWithList: typing.List[str], minWeight: float = 0.5) -> typing.List[str]:
	weightList = [(
	    compareWith,
	    getMatchingWeight(word, compareWith),
	) for compareWith in compareWithList]
	weightFilteredList = [(compareWith, weight) for compareWith, weight in weightList if weight >= minWeight]
	weightSortedList = sorted(weightFilteredList, key=lambda item: item[1], reverse=True)

	return [compareWith for compareWith, _ in weightSortedList]
