#include "bzd/container/impl/circular_list.h"
#include "bzd/test/sync_point.h"

#include "cc_test/test.h"

class DummyElement : public bzd::impl::CircularListElement
{
public:
	DummyElement(bzd::SizeType value) : value_{value} {}
	DummyElement(DummyElement&&) = default;

	bzd::SizeType value_;
};
/*
TEST(CircularList, simple)
{
	DummyElement elements[10]{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	bzd::impl::CircularList<DummyElement> list;

	{
		const auto result = list.insert(&elements[0]);
		EXPECT_TRUE(result);
	}

	{
		const auto& result = list.get();
		EXPECT_TRUE(result);
		EXPECT_EQ((*result).value_, elements[0].value_);
	}

	{
		const auto result = list.remove(&elements[0]);
		EXPECT_TRUE(result);
	}
}
*/
template <class T>
void insertWhileInsertDoWork()
{
	DummyElement a{1};
	DummyElement b{2};
	DummyElement c{3};
	bzd::impl::CircularList<DummyElement> list;
	list.insert(&a);
	using SyncPoint = bzd::test::SyncPoint<struct concurrency>;

	std::thread workerInsertB([&list, &b]() {
		const auto result = list.insert<T, typename SyncPoint::template Type<1>, typename SyncPoint::template Type<4>>(&b);
		EXPECT_TRUE(result);
	});
	std::thread workerInsertC([&list, &c]() {
		typename SyncPoint::template Type<2>();
		const auto result = list.insert(&c);
		EXPECT_TRUE(result);
		typename SyncPoint::template Type<3>();
	});

	workerInsertB.join();
	workerInsertC.join();

	const auto result = list.sanityCheck([](const auto&) -> bool {
		return true;
	});
	EXPECT_TRUE(result);
	EXPECT_EQ(*result, 3);
}

TEST(CircularList, insertWhileInsert)
{
	insertWhileInsertDoWork<bzd::impl::ListInjectPoint1>();
	insertWhileInsertDoWork<bzd::impl::ListInjectPoint2>();
	insertWhileInsertDoWork<bzd::impl::ListInjectPoint3>();
	insertWhileInsertDoWork<bzd::impl::ListInjectPoint4>();
	insertWhileInsertDoWork<bzd::impl::ListInjectPoint5>();
}

template <class T>
void removeWhileInsertDoWork()
{
		DummyElement a{1};
		DummyElement b{2};
		bzd::impl::CircularList<DummyElement> list;
		list.insert(&a);
		using SyncPoint = bzd::test::SyncPoint<struct concurrency>;

		std::thread workerInsert([&list, &b]() {
			const auto result = list.insert<T, typename SyncPoint::template Type<1>, typename SyncPoint::template Type<4>>(&b);
			EXPECT_TRUE(result);
		});
		std::thread workerRemove([&list, &a]() {
			typename SyncPoint::template Type<2>();
			const auto result = list.remove(&a);
			EXPECT_TRUE(result);
			typename SyncPoint::template Type<3>();
		});

		workerInsert.join();
		workerRemove.join();

		const auto result = list.sanityCheck([](const auto&) -> bool {
			return true;
		});
		EXPECT_TRUE(result);
		EXPECT_EQ(*result, 1);
}

TEST(CircularList, removeWhileInsert)
{
	removeWhileInsertDoWork<bzd::impl::ListInjectPoint1>();
	removeWhileInsertDoWork<bzd::impl::ListInjectPoint2>();
	removeWhileInsertDoWork<bzd::impl::ListInjectPoint3>();
	removeWhileInsertDoWork<bzd::impl::ListInjectPoint4>();
	removeWhileInsertDoWork<bzd::impl::ListInjectPoint5>();
}

template <class T>
void insertWhileRemoveDoWork()
{
		DummyElement a{1};
		DummyElement b{2};
		bzd::impl::CircularList<DummyElement> list;
		list.insert(&a);
		using SyncPoint = bzd::test::SyncPoint<struct concurrency>;

		std::thread workerInsert([&list, &b]() {
			typename SyncPoint::template Type<2>();
			const auto result = list.insert(&b);
			EXPECT_TRUE(result);
			typename SyncPoint::template Type<3>();
		});
		std::thread workerRemove([&list, &a]() {
			const auto result = list.remove<T, typename SyncPoint::template Type<1>, typename SyncPoint::template Type<4>>(&a);
			EXPECT_TRUE(result);
		});

		workerInsert.join();
		workerRemove.join();

		const auto result = list.sanityCheck([](const auto&) -> bool {
			return true;
		});
		EXPECT_TRUE(result);
		EXPECT_EQ(*result, 1);
}

TEST(CircularList, insertWhileRemove)
{
	insertWhileRemoveDoWork<bzd::impl::ListInjectPoint1>();
	insertWhileRemoveDoWork<bzd::impl::ListInjectPoint2>();
	//insertWhileRemoveDoWork<bzd::impl::ListInjectPoint3>();
	insertWhileRemoveDoWork<bzd::impl::ListInjectPoint4>();
}


#include <thread>
#include <iostream>
#include <vector>
#include <set>
#include <mutex>

TEST(CircularList, insertionStress)
{
	constexpr bzd::SizeType nbIterations = 100000;
	constexpr bzd::SizeType nbElements = 4;
	static bzd::Atomic<bzd::UInt8Type> inserted[nbElements]{};
	srand(time(NULL));

	// List
	bzd::impl::CircularList<DummyElement> list;

	// Elements
	std::vector<DummyElement> elements;
	for (bzd::SizeType i = 0; i<nbElements; ++i) {
		elements.push_back(DummyElement{i});
	}

	const auto workloadInsert = [&list, &elements]() {
		int counter = nbIterations;
		while (--counter)
		{
			auto& element = elements[rand() % elements.size()];

			//if (inserted[element.value_].load() > 0) {
			//	std::this_thread::yield();
			//	continue;
			//}

			std::cout << "Trying to insert " << element.value_ << std::endl;
			const auto result = list.insert(&element);
			if (!result)
			{
				ASSERT_EQ(result.error(), bzd::impl::ListErrorType::elementAlreadyInserted);
			}
			else
			{
				++inserted[element.value_];
			}
		}
	};

	const auto workloadRemove = [&list, &elements]() {
		int counter = nbIterations;
		while (--counter)
		{
			auto& element = elements[rand() % nbElements];

			if (inserted[element.value_].load() == 0) {
				std::this_thread::yield();
				continue;
			}

			std::cout << "Trying to remove " << element.value_ << std::endl;
			const auto result = list.remove(&element);
			if (!result)
			{
				ASSERT_TRUE(result.error() == bzd::impl::ListErrorType::elementAlreadyRemoved);
			}
			else
			{
				--inserted[element.value_];
			}
		}
	};
	(void)workloadRemove;

	std::thread worker1(workloadInsert);
	std::thread worker2(workloadRemove);
	std::thread worker3(workloadInsert);
	std::thread worker4(workloadInsert);

	worker1.join();
	worker2.join();
	worker3.join();
	worker4.join();

	const auto result = list.sanityCheck([](const auto& element) -> bool {
		//std::cout << "Checking " << element.value_ << std::endl;
		const auto value = inserted[element.value_].load();
		if (value == 0) {
			std::cout << "Value not present " << element.value_ << std::endl;
			return false;
		}
		inserted[element.value_].store(0);
		return true;
	});
	EXPECT_TRUE(result);

	// Ensure everything is empty
	for (bzd::SizeType i = 0; i < nbElements; ++i) {
		EXPECT_EQ(inserted[i].load(), 0);
	}
}

