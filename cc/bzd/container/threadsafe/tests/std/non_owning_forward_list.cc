#include "cc/bzd/container/threadsafe/tests/non_owning_forward_list_for_test.hh"
#include "cc/bzd/test/sync_point.hh"
#include "cc/bzd/test/test.hh"
#include "cc/bzd/utility/ignore.hh"

#include <iostream>
#include <mutex>
#include <set>
#include <thread>
#include <vector>

template <class T>
void insertWhileInsertDoWork()
{
	bzd::test::ListElementMultiContainer a{1};
	bzd::test::ListElementMultiContainer b{2};
	bzd::test::ListElementMultiContainer c{3};
	bzd::test::NonOwningForwardList<bzd::test::ListElementMultiContainer> list;
	bzd::ignore = list.pushFront(a);
	using SyncPoint = bzd::test::SyncPoint<struct concurrency>;

	std::thread workerInsertB([&list, &b]() {
		const auto result = list.pushFront<T, typename SyncPoint::template Type<1>, typename SyncPoint::template Type<4>>(b);
		EXPECT_TRUE(result);
	});
	std::thread workerInsertC([&list, &c]() {
		typename SyncPoint::template Type<2>();
		const auto result = list.pushFront(c);
		EXPECT_TRUE(result);
		typename SyncPoint::template Type<3>();
	});

	workerInsertB.join();
	workerInsertC.join();

	const auto result = list.sanityCheck([](const auto&) -> bool { return true; });
	EXPECT_EQ(result, 3U);
	EXPECT_EQ(list.size(), 3U);
}

TEST(NonOwningForwardList, InsertWhileInsert)
{
	insertWhileInsertDoWork<bzd::test::InjectPoint0>();
	insertWhileInsertDoWork<bzd::test::InjectPoint1>();
	insertWhileInsertDoWork<bzd::test::InjectPoint2>();
	insertWhileInsertDoWork<bzd::test::InjectPoint3>();
}

template <class T>
void removeWhileInsertDoWork()
{
	bzd::test::ListElementMultiContainer a{1};
	bzd::test::ListElementMultiContainer b{2};
	bzd::test::NonOwningForwardList<bzd::test::ListElementMultiContainer> list;
	bzd::ignore = list.pushFront(a);
	using SyncPoint = bzd::test::SyncPoint<struct concurrency>;

	std::thread workerInsert([&list, &b]() {
		const auto result = list.pushFront<T, typename SyncPoint::template Type<1>, typename SyncPoint::template Type<4>>(b);
		EXPECT_TRUE(result);
	});
	std::thread workerRemove([&list, &a]() {
		typename SyncPoint::template Type<2>();
		const auto result = list.pop(a);
		EXPECT_TRUE(result);
		typename SyncPoint::template Type<3>();
	});

	workerInsert.join();
	workerRemove.join();

	const auto result = list.sanityCheck([](const auto&) -> bool { return true; });
	EXPECT_EQ(result, 1U);
	EXPECT_EQ(list.size(), 1U);
}

TEST(NonOwningForwardList, RemoveWhileInsert)
{
	removeWhileInsertDoWork<bzd::test::InjectPoint0>();
	removeWhileInsertDoWork<bzd::test::InjectPoint1>();
	removeWhileInsertDoWork<bzd::test::InjectPoint2>();
	removeWhileInsertDoWork<bzd::test::InjectPoint3>();
}

template <class T>
void insertWhileRemoveDoWork()
{
	bzd::test::ListElementMultiContainer a{1};
	bzd::test::ListElementMultiContainer b{2};
	bzd::test::NonOwningForwardList<bzd::test::ListElementMultiContainer> list;
	bzd::ignore = list.pushFront(a);
	using SyncPoint = bzd::test::SyncPoint<struct concurrency>;

	std::thread workerInsert([&list, &b]() {
		typename SyncPoint::template Type<2>();
		const auto result = list.pushFront(b);
		EXPECT_TRUE(result);
		typename SyncPoint::template Type<3>();
	});
	std::thread workerRemove([&list, &a]() {
		const auto result = list.pop<T, typename SyncPoint::template Type<1>, typename SyncPoint::template Type<4>>(a);
		EXPECT_TRUE(result);
	});

	workerInsert.join();
	workerRemove.join();

	const auto result = list.sanityCheck([](const auto&) -> bool { return true; });
	EXPECT_EQ(result, 1U);
	EXPECT_EQ(list.size(), 1U);
}

TEST(NonOwningForwardList, InsertWhileRemove)
{
	insertWhileRemoveDoWork<bzd::test::InjectPoint0>();
	insertWhileRemoveDoWork<bzd::test::InjectPoint1>();
	insertWhileRemoveDoWork<bzd::test::InjectPoint2>();
	insertWhileRemoveDoWork<bzd::test::InjectPoint3>();
	insertWhileRemoveDoWork<bzd::test::InjectPoint4>();
	insertWhileRemoveDoWork<bzd::test::InjectPoint5>();
}

template <class T>
void removeWhileRemoveLeftDoWork()
{
	bzd::test::ListElementMultiContainer a{1};
	bzd::test::ListElementMultiContainer b{2};
	bzd::test::NonOwningForwardList<bzd::test::ListElementMultiContainer> list;
	bzd::ignore = list.pushFront(a);
	bzd::ignore = list.pushFront(b);
	using SyncPoint = bzd::test::SyncPoint<struct concurrency>;

	std::thread workerRemove1([&list, &b]() {
		typename SyncPoint::template Type<2>();
		const auto result = list.pop(b);
		EXPECT_TRUE(result);
		typename SyncPoint::template Type<3>();
	});
	std::thread workerRemove2([&list, &a]() {
		const auto result = list.pop<T, typename SyncPoint::template Type<1>, typename SyncPoint::template Type<4>>(a);
		EXPECT_TRUE(result);
	});

	workerRemove1.join();
	workerRemove2.join();

	const auto result = list.sanityCheck([](const auto&) -> bool { return true; });
	EXPECT_EQ(result, 0U);
	EXPECT_EQ(list.size(), 0U);
}

TEST(NonOwningForwardList, RemoveWhileRemoveLeft)
{
	removeWhileRemoveLeftDoWork<bzd::test::InjectPoint0>();
	removeWhileRemoveLeftDoWork<bzd::test::InjectPoint1>();
	removeWhileRemoveLeftDoWork<bzd::test::InjectPoint2>();
	removeWhileRemoveLeftDoWork<bzd::test::InjectPoint3>();
	removeWhileRemoveLeftDoWork<bzd::test::InjectPoint4>();
	removeWhileRemoveLeftDoWork<bzd::test::InjectPoint5>();
}

template <class T>
void removeWhileRemoveRightDoWork()
{
	bzd::test::ListElementMultiContainer a{1};
	bzd::test::ListElementMultiContainer b{2};
	bzd::test::NonOwningForwardList<bzd::test::ListElementMultiContainer> list;
	bzd::ignore = list.pushFront(a);
	bzd::ignore = list.pushFront(b);
	using SyncPoint = bzd::test::SyncPoint<struct concurrency>;

	std::thread workerRemove1([&list, &a]() {
		typename SyncPoint::template Type<2>();
		const auto result = list.pop(a);
		EXPECT_TRUE(result);
		typename SyncPoint::template Type<3>();
	});
	std::thread workerRemove2([&list, &b]() {
		const auto result = list.pop<T, typename SyncPoint::template Type<1>, typename SyncPoint::template Type<4>>(b);
		EXPECT_TRUE(result);
	});

	workerRemove1.join();
	workerRemove2.join();

	const auto result = list.sanityCheck([](const auto&) -> bool { return true; });
	EXPECT_EQ(result, 0U);
	EXPECT_EQ(list.size(), 0U);
}

TEST(NonOwningForwardList, RemoveWhileRemoveRight)
{
	removeWhileRemoveRightDoWork<bzd::test::InjectPoint0>();
	removeWhileRemoveRightDoWork<bzd::test::InjectPoint1>();
	removeWhileRemoveRightDoWork<bzd::test::InjectPoint2>();
	removeWhileRemoveRightDoWork<bzd::test::InjectPoint3>();
	removeWhileRemoveRightDoWork<bzd::test::InjectPoint4>();
	removeWhileRemoveRightDoWork<bzd::test::InjectPoint5>();
}

constexpr bzd::Size nbIterations = 100000;
constexpr bzd::Size nbElements = 10;

template <class T>
struct Data
{
	std::vector<T>& elements;

	bzd::test::NonOwningForwardList<T> list{};
	bzd::Atomic<bzd::UInt16> inserted[nbElements]{};
	bzd::Atomic<bzd::Size> insertion{0};
	bzd::Atomic<bzd::Size> removal{0};

	void sanityCheck()
	{
		const auto result = list.sanityCheck([this](const auto& element) -> bool {
			const auto value = inserted[element.value_].load();
			if (value == 0)
			{
				std::cout << "Value not present " << element.value_ << std::endl;
				return false;
			}
			inserted[element.value_].store(0);
			return true;
		});
		bzd::ignore = result;

		// Ensure everything is empty
		for (bzd::Size i = 0; i < nbElements; ++i)
		{
			EXPECT_EQ(inserted[i].load(), 0);
		}
	}
};

template <class T>
void workloadInsert(Data<T>* pData)
{
	int counter = nbIterations;
	while (--counter)
	{
		auto& element = pData->elements[rand() % nbElements];

		const auto result = pData->list.pushFront(element);
		if (!result)
		{
			ASSERT_EQ(result.error(), bzd::threadsafe::NonOwningForwardListErrorType::elementAlreadyInserted);
		}
		else
		{
			++(pData->inserted[element.value_]);
			++pData->insertion;
		}
	}
}

template <class T>
void workloadRemove(Data<T>* pData)
{
	int counter = nbIterations;
	while (--counter)
	{
		auto& element = pData->elements[rand() % nbElements];

		const auto result = pData->list.pop(element);
		if (!result)
		{
			ASSERT_TRUE(result.error() == bzd::threadsafe::NonOwningForwardListErrorType::elementAlreadyRemoved ||
						result.error() == bzd::threadsafe::NonOwningForwardListErrorType::notFound);
		}
		else
		{
			--(pData->inserted[element.value_]);
			++pData->removal;
		}
	}
}

TEST(NonOwningForwardList, InsertionStressMultiContainer)
{
	srand(time(NULL));

	// Elements
	static std::vector<bzd::test::ListElementMultiContainer> elements;
	for (bzd::Size i = 0; i < nbElements; ++i)
	{
		elements.push_back(bzd::test::ListElementMultiContainer{i});
	}
	ASSERT_EQ(nbElements, elements.size());

	// List
	Data<bzd::test::ListElementMultiContainer> data1{elements};
	Data<bzd::test::ListElementMultiContainer> data2{elements};

	std::thread worker1(workloadInsert<bzd::test::ListElementMultiContainer>, &data1);
	std::thread worker2(workloadInsert<bzd::test::ListElementMultiContainer>, &data1);
	std::thread worker3(workloadRemove<bzd::test::ListElementMultiContainer>, &data1);
	std::thread worker4(workloadRemove<bzd::test::ListElementMultiContainer>, &data1);
	std::thread worker5(workloadInsert<bzd::test::ListElementMultiContainer>, &data2);
	std::thread worker6(workloadInsert<bzd::test::ListElementMultiContainer>, &data2);
	std::thread worker7(workloadRemove<bzd::test::ListElementMultiContainer>, &data2);
	std::thread worker8(workloadRemove<bzd::test::ListElementMultiContainer>, &data2);

	worker1.join();
	worker2.join();
	worker3.join();
	worker4.join();
	worker5.join();
	worker6.join();
	worker7.join();
	worker8.join();

	for (bzd::Size i = 0; i < nbElements; ++i)
	{
		std::cout << i << ": ";
		data1.list.printNode(&elements[i]);
	}
	std::cout << std::endl;
	std::cout << "data1.sanityCheck();" << std::endl;
	std::cout << "Insertion: " << data1.insertion.load() << std::endl;
	std::cout << "Removal: " << data1.removal.load() << std::endl;
	data1.sanityCheck();
	std::cout << "data2.sanityCheck();" << std::endl;
	std::cout << "Insertion: " << data2.insertion.load() << std::endl;
	std::cout << "Removal: " << data2.removal.load() << std::endl;
	data2.sanityCheck();

	EXPECT_TRUE(data1.insertion.load() > 0 || data2.insertion.load() > 0);
}

TEST(NonOwningForwardList, InsertionStress)
{
	srand(time(NULL));

	// Elements
	static std::vector<bzd::test::ListElement> elements;
	for (bzd::Size i = 0; i < nbElements; ++i)
	{
		elements.push_back(bzd::test::ListElement{i});
	}
	ASSERT_EQ(nbElements, elements.size());

	// List
	Data<bzd::test::ListElement> data1{elements};

	std::thread worker1(workloadInsert<bzd::test::ListElement>, &data1);
	std::thread worker2(workloadInsert<bzd::test::ListElement>, &data1);
	std::thread worker3(workloadRemove<bzd::test::ListElement>, &data1);
	std::thread worker4(workloadRemove<bzd::test::ListElement>, &data1);
	std::thread worker5(workloadInsert<bzd::test::ListElement>, &data1);
	std::thread worker6(workloadInsert<bzd::test::ListElement>, &data1);
	std::thread worker7(workloadRemove<bzd::test::ListElement>, &data1);
	std::thread worker8(workloadRemove<bzd::test::ListElement>, &data1);

	worker1.join();
	worker2.join();
	worker3.join();
	worker4.join();
	worker5.join();
	worker6.join();
	worker7.join();
	worker8.join();

	for (bzd::Size i = 0; i < nbElements; ++i)
	{
		std::cout << i << ": ";
		data1.list.printNode(&elements[i]);
	}
	std::cout << std::endl;

	std::cout << "data1.sanityCheck();" << std::endl;
	std::cout << "Insertion: " << data1.insertion.load() << std::endl;
	std::cout << "Removal: " << data1.removal.load() << std::endl;
	data1.sanityCheck();
}

TEST(NonOwningForwardList, PopToDiscard)
{
	for (bzd::Size iteration = 0; iteration < 1000; ++iteration)
	{
		bzd::Array<std::thread, 10> threads;
		bzd::test::NonOwningForwardList<bzd::test::ListElementDiscard> data{};
		;

		for (auto& thread : threads)
		{
			thread = std::thread{[&data]() {
				bzd::test::ListElementDiscard element;
				const auto result1 = data.pushFront(element);
				EXPECT_TRUE(result1);
				const auto result2 = data.popToDiscard(element);
				EXPECT_TRUE(result2);
			}};
		}

		for (auto& thread : threads)
		{
			thread.join();
		}
	}
}
