#include "cc/bzd/container/threadsafe/non_owning_queue_spin.hh"
#include "cc/bzd/test/exec_point.hh"
#include "cc/bzd/test/test.hh"

namespace bzd::test {

class ListElement : public bzd::threadsafe::NonOwningQueueElement
{
public:
	ListElement() = default;
	ListElement(bzd::Size value) : value{value} {}
	bzd::Size value{0};
};

namespace {
ListElement elements[10]{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
}

TEST(NonOwningQueue, scenario1, (int, void))
{
	threadsafe::NonOwningQueue<ListElement> queue;

	queue.push(elements[0]);
	queue.push(elements[1]);
	queue.push(elements[2]);
	queue.push(elements[3]);

	for (bzd::Size i = 0; i < 4; ++i)
	{
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_TRUE(maybeElement.value().value == i);
	}
	{
		auto maybeElement = queue.pop();
		EXPECT_FALSE(maybeElement);
	}
}

TEST(NonOwningQueue, scenario2)
{
	threadsafe::NonOwningQueue<ListElement> queue;

	queue.push(elements[0]);
	queue.push(elements[1]);
	bzd::ignore = queue.pop();
	queue.push(elements[2]);
	queue.push(elements[3]);
	bzd::ignore = queue.pop();

	{
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_TRUE(maybeElement.value().value == 2);
	}
	{
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_TRUE(maybeElement.value().value == 3);
	}
	{
		auto maybeElement = queue.pop();
		EXPECT_FALSE(maybeElement);
	}
}

// ---- Concurrent push ----
/*
TEST(NonOwningQueue, ConcurrentPushNoElement)
{
	threadsafe::NonOwningQueue<ListElement> queue;

	ExecPoint pushNewElement{[&]() { queue.push(elements[1]); }};

	queue.push<bzd::test::InjectPoint0, decltype(pushNewElement)>(elements[0]);

	{
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 0U);
	}
	{
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 1U);
	}
}

TEST(NonOwningQueue, ConcurrentPushElement)
{
	threadsafe::NonOwningQueue<ListElement> queue;

	ExecPoint pushNewElement{[&]() { queue.push(elements[2]); }};

	queue.push(elements[0]);
	queue.push<bzd::test::InjectPoint0, decltype(pushNewElement)>(elements[1]);

	{
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 0U);
	}
	{
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 1U);
	}
	{
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 2U);
	}
}

// ---- Concurrent pop ----

// Concurrent pop() are not possible, during the pop operation, the tail
// is set to nullptr.
TEST(NonOwningQueue, ConcurrentPop0)
{
	threadsafe::NonOwningQueue<ListElement> queue;

	ExecPoint popElement{[&]() {
		auto maybeElement = queue.pop();
		EXPECT_FALSE(maybeElement);
	}};

	queue.push(elements[0]);
	queue.push(elements[1]);

	{
		auto maybeElement = queue.pop<bzd::test::InjectPoint0, decltype(popElement)>();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 0U);
	}
	{
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 1U);
	}
}

TEST(NonOwningQueue, ConcurrentPop1)
{
	threadsafe::NonOwningQueue<ListElement> queue;

	// During a short period of time poping new element is disabled.
	ExecPoint popElement{[&]() {
		auto maybeElement = queue.pop();
		EXPECT_FALSE(maybeElement);
	}};

	queue.push(elements[0]);
	queue.push(elements[1]);

	{
		auto maybeElement = queue.pop<bzd::test::InjectPoint1, decltype(popElement)>();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 0U);
	}
	{
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 1U);
	}
}

TEST(NonOwningQueue, ConcurrentPop2)
{
	threadsafe::NonOwningQueue<ListElement> queue;

	// During a short period of time poping new element is disabled.
	ExecPoint popElement{[&]() {
		auto maybeElement = queue.pop();
		EXPECT_FALSE(maybeElement);
	}};

	queue.push(elements[0]);
	queue.push(elements[1]);

	{
		auto maybeElement = queue.pop<bzd::test::InjectPoint2, decltype(popElement)>();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 0U);
	}
	{
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 1U);
	}
}

// ---- Concurrent pop and push ----

TEST(NonOwningQueue, ConcurrentPopPush0)
{
	threadsafe::NonOwningQueue<ListElement> queue;

	ExecPoint pushNewElement{[&]() { queue.push(elements[1]); }};

	queue.push(elements[0]);

	{
		auto maybeElement = queue.pop<bzd::test::InjectPoint0, decltype(pushNewElement)>();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 0U);
	}
	{
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 1U);
	}
}

TEST(NonOwningQueue, ConcurrentPopPush1)
{
	threadsafe::NonOwningQueue<ListElement> queue;

	ExecPoint pushNewElement{[&]() { queue.push(elements[1]); }};

	queue.push(elements[0]);

	{
		auto maybeElement = queue.pop<bzd::test::InjectPoint1, decltype(pushNewElement)>();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 0U);
	}
	{
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 1U);
	}
}

TEST(NonOwningQueue, ConcurrentPopPush2)
{
	threadsafe::NonOwningQueue<ListElement> queue;

	ExecPoint pushNewElement{[&]() { queue.push(elements[1]); }};

	queue.push(elements[0]);

	{
		auto maybeElement = queue.pop<bzd::test::InjectPoint2, decltype(pushNewElement)>();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 0U);
	}
	{
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 1U);
	}
}

// ---- Concurrent push and pop ----

TEST(NonOwningQueue, ConcurrentPushPopNoElement)
{
	threadsafe::NonOwningQueue<ListElement> queue;

	ExecPoint popElement{[&]() {
		auto maybeElement = queue.pop();
		EXPECT_FALSE(maybeElement);
	}};

	queue.push<bzd::test::InjectPoint0, decltype(popElement)>(elements[0]);

	{
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 0U);
	}
}

TEST(NonOwningQueue, ConcurrentPushPopElement)
{
	threadsafe::NonOwningQueue<ListElement> queue;

	ExecPoint popElement{[&]() {
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 0U);
	}};

	queue.push(elements[0]);
	queue.push<bzd::test::InjectPoint0, decltype(popElement)>(elements[1]);

	{
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 1U);
	}
}

// ---- Concurrent push and pop and push ----

TEST(NonOwningQueue, ConcurrentPushPopPush0)
{
	threadsafe::NonOwningQueue<ListElement> queue;

	ExecPoint pushNewElement{[&]() { queue.push(elements[2]); }};

	ExecPoint popElement{[&]() {
		auto maybeElement = queue.pop<bzd::test::InjectPoint0, decltype(pushNewElement)>();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 0U);
	}};

	queue.push(elements[0]);
	queue.push<bzd::test::InjectPoint0, decltype(popElement)>(elements[1]);

	{
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 1U);
	}
	{
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 2U);
	}
}

TEST(NonOwningQueue, ConcurrentPushPopPush1)
{
	threadsafe::NonOwningQueue<ListElement> queue;

	ExecPoint pushNewElement{[&]() { queue.push(elements[2]); }};

	ExecPoint popElement{[&]() {
		auto maybeElement = queue.pop<bzd::test::InjectPoint1, decltype(pushNewElement)>();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 0U);
	}};

	queue.push(elements[0]);
	queue.push<bzd::test::InjectPoint0, decltype(popElement)>(elements[1]);

	{
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 1U);
	}
	{
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 2U);
	}
}

TEST(NonOwningQueue, ConcurrentPushPopPush2)
{
	threadsafe::NonOwningQueue<ListElement> queue;

	ExecPoint pushNewElement{[&]() { queue.push(elements[2]); }};

	ExecPoint popElement{[&]() {
		auto maybeElement = queue.pop<bzd::test::InjectPoint2, decltype(pushNewElement)>();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 0U);
	}};

	queue.push(elements[0]);
	queue.push<bzd::test::InjectPoint0, decltype(popElement)>(elements[1]);

	{
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 1U);
	}
	{
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 2U);
	}
}

// ---- Concurrent push and push and pop ----

TEST(NonOwningQueue, ConcurrentPushPushPop)
{
	threadsafe::NonOwningQueue<ListElement> queue;

	ExecPoint popElement{[&]() {
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 0U);
	}};

	ExecPoint pushNewElement{[&]() { queue.push<bzd::test::InjectPoint0, decltype(popElement)>(elements[2]); }};

	queue.push(elements[0]);
	queue.push<bzd::test::InjectPoint0, decltype(pushNewElement)>(elements[1]);

	{
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 1U);
	}
	{
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 2U);
	}
}

// ---- Concurrent pop and push and push ----

TEST(NonOwningQueue, ConcurrentPopPushPush0)
{
	threadsafe::NonOwningQueue<ListElement> queue;

	ExecPoint pushNewElement2{[&]() { queue.push(elements[2]); }};
	ExecPoint pushNewElement1{[&]() { queue.push<bzd::test::InjectPoint0, decltype(pushNewElement2)>(elements[1]); }};

	queue.push(elements[0]);

	{
		auto maybeElement = queue.pop<bzd::test::InjectPoint0, decltype(pushNewElement1)>();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 0U);
	}
	{
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 1U);
	}
	{
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 2U);
	}
}

TEST(NonOwningQueue, ConcurrentPopPushPush1)
{
	threadsafe::NonOwningQueue<ListElement> queue;

	ExecPoint pushNewElement2{[&]() { queue.push(elements[2]); }};
	ExecPoint pushNewElement1{[&]() { queue.push<bzd::test::InjectPoint0, decltype(pushNewElement2)>(elements[1]); }};

	queue.push(elements[0]);

	{
		auto maybeElement = queue.pop<bzd::test::InjectPoint1, decltype(pushNewElement1)>();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 0U);
	}
	{
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 1U);
	}
	{
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 2U);
	}
}

TEST(NonOwningQueue, ConcurrentPopPushPush2)
{
	threadsafe::NonOwningQueue<ListElement> queue;

	ExecPoint pushNewElement2{[&]() { queue.push(elements[2]); }};
	ExecPoint pushNewElement1{[&]() { queue.push<bzd::test::InjectPoint0, decltype(pushNewElement2)>(elements[1]); }};

	queue.push(elements[0]);

	{
		auto maybeElement = queue.pop<bzd::test::InjectPoint2, decltype(pushNewElement1)>();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 0U);
	}
	{
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 1U);
	}
	{
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 2U);
	}
}

// ---- Concurrent pop and 2 push ----

TEST(NonOwningQueue, ConcurrentPop2PushPush0)
{
	threadsafe::NonOwningQueue<ListElement> queue;
	ListElement elements[10]{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

	ExecPoint pushNewElement{[&]() {
		queue.print();
		queue.push(elements[2]);
		queue.print();
	}}; //queue.push(elements[2]); queue.push(elements[3]); }};

	queue.push(elements[0]);
	queue.push(elements[1]);
	queue.print();

	{
		auto maybeElement = queue.pop<bzd::test::InjectPoint0, decltype(pushNewElement)>();
		queue.print();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 0U);
	}
	{
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 1U);
	}
	{
		auto maybeElement = queue.pop();
		EXPECT_TRUE(maybeElement);
		EXPECT_EQ(maybeElement.value().value, 2U);
	}
}
*/
} // namespace bzd::test
