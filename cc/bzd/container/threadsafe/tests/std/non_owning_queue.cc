#include "cc/bzd/container/threadsafe/non_owning_queue_spin.hh"
#include "cc/bzd/test/test.hh"

#include <iostream>
#include <mutex>
#include <set>
#include <thread>
#include <vector>

namespace bzd::test {

class ListElement : public bzd::threadsafe::NonOwningQueueElement
{
public:
	ListElement() = default;
	ListElement(Size v) : value{v} {}
	ListElement(const ListElement&) = default;
	bzd::Size value{0};
};

TEST(NonOwningQueue, StressPush)
{
	// Coonfiguration.
	static constexpr Size nbPushThreads{3};
	static constexpr Size nbPopThreads{2};
	static constexpr Size maxElements{10};
	static constexpr Size maxIterations{100};

	bzd::Atomic<Size> counter{0};
	bzd::Atomic<Size> iteration{0};
	std::vector<std::thread> workers;
	std::vector<ListElement> elements;

	srand(time(NULL));

	for (Size i = 0; i < maxElements; ++i)
	{
		elements.push_back(ListElement{i});
	}

	// Queue
	threadsafe::NonOwningQueue<ListElement> queue;

	// Create pushers
	for (Size i = 0; i < nbPushThreads; ++i)
	{
		constexpr Size increment = maxElements / nbPushThreads;
		const Size start = i * increment;
		const Size end = (i + 1 == nbPushThreads) ? maxElements : ((i + 1) * increment);
		workers.emplace_back([&, end, start]() {
			for (Size expectedIteration = 0; expectedIteration < maxIterations; ++expectedIteration)
			{
				// Wait for the iteration to start.
				while (iteration.load() != expectedIteration)
				{
				};
				// Push the elements.
				for (Size j = start; j < end; ++j)
				{
					queue.pushFront(elements[j]);
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
		workers.emplace_back([&, isMaster]() {
			for (Size expectedIteration = 0; expectedIteration < maxIterations; ++expectedIteration)
			{
				while (expectedIteration == iteration.load())
				{
					if (queue.popBack())
					{
						++popped;
					}

					if (isMaster && popped.load() == maxElements)
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
