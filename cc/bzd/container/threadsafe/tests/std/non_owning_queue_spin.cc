#include "cc/bzd/container/threadsafe/non_owning_queue_spin.hh"

#include "cc/bzd/container/threadsafe/tests/std/utils.hh"
#include "cc/bzd/test/multithread.hh"

#include <vector>

namespace bzd::test {

class ListElement : public bzd::threadsafe::NonOwningQueueSpinElement
{
public:
	ListElement() = default;
	ListElement(Size v) : value{v} {}
	ListElement(const ListElement&) = default;
	bzd::Size value{0};
};

TEST(NonOwningQueueSpin, StressPush)
{
	srand(time(NULL));

	std::vector<ListElement> elements;
	for (Size i = 0; i < 10; ++i)
	{
		elements.push_back(ListElement{i});
	}

	// Queue
	threadsafe::NonOwningQueueSpin<ListElement> queue;
	makePushPopStressTest<3, 2, 1000>(elements, [&](ListElement& element) { queue.pushFront(element); }, [&]() { return queue.popBack(); });
}

} // namespace bzd::test
