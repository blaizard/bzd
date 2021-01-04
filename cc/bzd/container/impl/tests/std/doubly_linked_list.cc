#include "bzd/container/impl/doubly_linked_list.h"

#include "bzd/test/sync_point.h"
#include "cc_test/test.h"

#include <iostream>
#include <mutex>
#include <set>
#include <thread>
#include <vector>

class DummyElement : public bzd::impl::DoublyLinkedListElement
{
public:
	DummyElement(bzd::SizeType value) : value_{value} {}
	DummyElement(DummyElement&&) = default;

	bzd::SizeType value_;
};

template <class T>
void insertWhileInsertDoWork()
{
	DummyElement a{1};
	DummyElement b{2};
	DummyElement c{3};
	bzd::impl::DoublyLinkedList<DummyElement> list;
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

	const auto result = list.sanityCheck([](const auto&) -> bool { return true; });
	EXPECT_TRUE(result);
	EXPECT_EQ(*result, 3);
	EXPECT_EQ(list.size(), 3);
}

TEST(DoublyLinkedList, insertWhileInsert)
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
	bzd::impl::DoublyLinkedList<DummyElement> list;
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

	const auto result = list.sanityCheck([](const auto&) -> bool { return true; });
	EXPECT_TRUE(result);
	EXPECT_EQ(*result, 1);
	EXPECT_EQ(list.size(), 1);
}

TEST(DoublyLinkedList, removeWhileInsert)
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
	bzd::impl::DoublyLinkedList<DummyElement> list;
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

	const auto result = list.sanityCheck([](const auto&) -> bool { return true; });
	EXPECT_TRUE(result);
	EXPECT_EQ(*result, 1);
	EXPECT_EQ(list.size(), 1);
}

TEST(DoublyLinkedList, insertWhileRemove)
{
	insertWhileRemoveDoWork<bzd::impl::ListInjectPoint1>();
	insertWhileRemoveDoWork<bzd::impl::ListInjectPoint2>();
	insertWhileRemoveDoWork<bzd::impl::ListInjectPoint3>();
	insertWhileRemoveDoWork<bzd::impl::ListInjectPoint4>();
	insertWhileRemoveDoWork<bzd::impl::ListInjectPoint5>();
}

TEST(DoublyLinkedList, insertionStress)
{
	srand(time(NULL));

	constexpr bzd::SizeType nbIterations = 10000;
	constexpr bzd::SizeType nbElements = 4;

	struct Data
	{
		bzd::impl::DoublyLinkedList<DummyElement> list{};
		bzd::Atomic<bzd::UInt8Type> inserted[nbElements]{};

		void sanityCheck()
		{
			const auto result = list.sanityCheck([this](const auto& element) -> bool {
				// std::cout << "Checking " << element.value_ << std::endl;
				const auto value = inserted[element.value_].load();
				if (value == 0)
				{
					std::cout << "Value not present " << element.value_ << std::endl;
					return false;
				}
				inserted[element.value_].store(0);
				return true;
			});
			EXPECT_TRUE(result);

			// Ensure everything is empty
			for (bzd::SizeType i = 0; i < nbElements; ++i)
			{
				EXPECT_EQ(inserted[i].load(), 0);
			}
		}
	};

	// List
	Data data1{};
//	Data data2{};

	// Elements
	static std::vector<DummyElement> elements;
	for (bzd::SizeType i = 0; i < nbElements; ++i)
	{
		elements.push_back(DummyElement{i});
		std::cout << i << ": " << &elements[i] << std::endl;
	}
	ASSERT_EQ(nbElements, elements.size());

	const auto workloadInsert = [](Data* pData) {
		int counter = nbIterations;
		while (--counter)
		{
			auto& element = elements[rand() % nbElements];

			//std::cout << "insert " << &element << std::endl;

			const auto result = pData->list.insert(&element);
			if (!result)
			{
				ASSERT_EQ(result.error(), bzd::impl::ListErrorType::elementAlreadyInserted);
			}
			else
			{
				++(pData->inserted[element.value_]);
			}
		}
	};

	const auto workloadRemove = [](Data* pData) {
		int counter = nbIterations;
		while (--counter)
		{
			auto& element = elements[rand() % nbElements];

			//std::cout << "remove " << &element << std::endl;

			const auto result = pData->list.remove(&element);
			if (!result)
			{
				ASSERT_TRUE(result.error() == bzd::impl::ListErrorType::elementAlreadyRemoved);
			}
			else
			{
				--(pData->inserted[element.value_]);
			}
		}
	};

	std::thread worker1(workloadInsert, &data1);
	std::thread worker2(workloadInsert, &data1);
	std::thread worker3(workloadRemove, &data1);
	std::thread worker4(workloadInsert, &data1);
//	std::thread worker5(workloadInsert, &data2);
//	std::thread worker6(workloadRemove, &data2);

	worker1.join();
	worker2.join();
	worker3.join();
	worker4.join();
//	worker5.join();
//	worker6.join();

	for (bzd::SizeType i = 0; i < nbElements; ++i)
	{
		std::cout << i << ": ";
		data1.list.printNode(&elements[i]);
		// << &elements[i] << std::endl;
	}
	std::cout << std::endl;
	data1.sanityCheck();
//	data2.sanityCheck();
}
