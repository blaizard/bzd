#include "cc/bzd/container/threadsafe/non_owning_queue_spin.hh"

#include "cc/bzd/test/test.hh"

namespace bzd::test {

class ListElement : public bzd::threadsafe::NonOwningQueueSpinElement
{
public:
	ListElement() = default;
	ListElement(bzd::Size value) : value{value} {}
	bzd::Size value{0};
};

namespace {
ListElement elements[10]{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
}

TEST(NonOwningQueueSpin, scenario1, (int, void))
{
	threadsafe::NonOwningQueueSpin<ListElement> queue;

	queue.pushFront(elements[0]);
	queue.pushFront(elements[1]);
	queue.pushFront(elements[2]);
	queue.pushFront(elements[3]);

	for (bzd::Size i = 0; i < 4; ++i)
	{
		auto maybeElement = queue.popBack();
		EXPECT_TRUE(maybeElement);
		EXPECT_TRUE(maybeElement.value().value == i);
	}
	{
		auto maybeElement = queue.popBack();
		EXPECT_FALSE(maybeElement);
	}
}

TEST(NonOwningQueueSpin, scenario2)
{
	threadsafe::NonOwningQueueSpin<ListElement> queue;

	queue.pushFront(elements[0]);
	queue.pushFront(elements[1]);
	bzd::ignore = queue.popBack();
	queue.pushFront(elements[2]);
	queue.pushFront(elements[3]);
	bzd::ignore = queue.popBack();

	{
		auto maybeElement = queue.popBack();
		EXPECT_TRUE(maybeElement);
		EXPECT_TRUE(maybeElement.value().value == 2);
	}
	{
		auto maybeElement = queue.popBack();
		EXPECT_TRUE(maybeElement);
		EXPECT_TRUE(maybeElement.value().value == 3);
	}
	{
		auto maybeElement = queue.popBack();
		EXPECT_FALSE(maybeElement);
	}
}

} // namespace bzd::test
