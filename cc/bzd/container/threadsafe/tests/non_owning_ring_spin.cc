#include "cc/bzd/container/threadsafe/non_owning_ring_spin.hh"

#include "cc/bzd/test/test.hh"

namespace bzd::test {

class ListElement : public bzd::threadsafe::NonOwningRingSpinElement
{
public:
	ListElement() = default;
	ListElement(bzd::Size value) : value{value} {}
	bzd::Size value{0};
};

namespace {
ListElement elements[10]{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
}

TEST(NonOwningRingSpin, scenario1, (int, void))
{
	threadsafe::NonOwningRingSpin<ListElement> queue;

	queue.pushBack(elements[0]);
	queue.pushBack(elements[1]);
	queue.pushBack(elements[2]);
	queue.pushBack(elements[3]);

	for (bzd::Size i = 0; i < 4; ++i)
	{
		auto maybeElement = queue.popFront();
		EXPECT_TRUE(maybeElement);
		EXPECT_TRUE(maybeElement.value().value == i);
	}
	{
		auto maybeElement = queue.popFront();
		EXPECT_FALSE(maybeElement);
	}
}

TEST(NonOwningRingSpin, scenario2)
{
	threadsafe::NonOwningRingSpin<ListElement> queue;

	queue.pushBack(elements[0]);
	queue.pushBack(elements[1]);
	bzd::ignore = queue.popFront();
	queue.pushBack(elements[2]);
	queue.pushBack(elements[3]);
	bzd::ignore = queue.popFront();

	{
		auto maybeElement = queue.popFront();
		EXPECT_TRUE(maybeElement);
		EXPECT_TRUE(maybeElement.value().value == 2);
	}
	{
		auto maybeElement = queue.popFront();
		EXPECT_TRUE(maybeElement);
		EXPECT_TRUE(maybeElement.value().value == 3);
	}
	{
		auto maybeElement = queue.popFront();
		EXPECT_FALSE(maybeElement);
	}
}

} // namespace bzd::test
