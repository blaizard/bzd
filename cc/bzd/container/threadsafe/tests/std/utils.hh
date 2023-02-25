#include "cc/bzd/container/vector.hh"

#include <thread>
#include <vector>

namespace bzd::test {

template <Size nbPushThreads, Size nbPopThreads, Size nbIterations, class Element, class PushCallback, class PopCallback>
void makePushPopStressTest(std::vector<Element>& elements, PushCallback&& push, PopCallback&& pop)
{
	bzd::Atomic<Size> counter{0};
	bzd::Atomic<Size> iteration{0};
	bzd::Vector<std::thread, nbPushThreads + nbPopThreads> workers;

	// Create pushers
	for (Size i = 0; i < nbPushThreads; ++i)
	{
		const Size increment = elements.size() / nbPushThreads;
		const Size start = i * increment;
		const Size end = (i + 1 == nbPushThreads) ? elements.size() : ((i + 1) * increment);
		workers.emplaceBack([&, end, start]() {
			for (Size expectedIteration = 0; expectedIteration < nbIterations; ++expectedIteration)
			{
				// Wait for the iteration to start.
				while (iteration.load() != expectedIteration)
				{
				};
				// Push the elements.
				for (Size j = start; j < end; ++j)
				{
					push(elements[j]);
					++counter;
				}
			}
		});
	}

	// Create poppers
	bzd::Atomic<Size> popped{0};
	for (Size i = 0; i < nbPopThreads; ++i)
	{
		const Bool isMaster = (i == 0);
		workers.emplaceBack([&, isMaster]() {
			for (Size expectedIteration = 0; expectedIteration < nbIterations; ++expectedIteration)
			{
				while (expectedIteration == iteration.load())
				{
					if (pop())
					{
						++popped;
					}

					if (isMaster && popped.load() == elements.size())
					{
						popped.store(0);
						++iteration;
					}
				}
			}
		});
	}

	for (auto& thread : workers)
	{
		thread.join();
	}
}

} // namespace bzd::test
