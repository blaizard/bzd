#include "bzd/container/impl/circular_list.h"
#include "bzd/test/sync_points.h"

#include "cc_test/test.h"

class DummyElement : public bzd::impl::CircularListElement
{
public:
	DummyElement(bzd::SizeType value) : value_{value} {}
	DummyElement(DummyElement&&) = default;

	bzd::SizeType value_;
};

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

TEST(CircularList, concurrency)
{
	{
	/*	DummyElement a{1};
		DummyElement b{1};
		bzd::impl::CircularList<DummyElement> list;
		list.insert(&a);*/
		using SyncPoints = bzd::test::SyncPoints<struct concurrency>;

		std::thread worker1([]() {
			SyncPoints::Type<1>()();
			SyncPoints::make<3>();
			SyncPoints::make<5>();
			SyncPoints::make<6>();
			SyncPoints::make<8>();
			//list.insert<1, SyncPoint2, SyncPoint5>(&b);
		});
		std::thread worker2([]() {
			SyncPoints::make<2>();
			SyncPoints::make<4>();
			SyncPoints::make<7>();
			SyncPoints::make<9>();
			//const auto result = list.remove(&a);
			//EXPECT_TRUE(result);
		});


		//list.insert<SyncPoint>(&b);
		//const auto result = list.remove(&a);
		worker1.join();
		worker2.join();
		EXPECT_TRUE(true);
	}

	{
		DummyElement a{1};
		DummyElement b{1};
		bzd::impl::CircularList<DummyElement> list;
		using SyncPoints = bzd::test::SyncPoints<struct concurrency>;

		list.insert(&a);
		list.insert<bzd::impl::Point1, SyncPoints::Type<2>>(&b);
		const auto result = list.remove(&a);
		EXPECT_TRUE(result);
	}

	{
		DummyElement a{1};
		DummyElement b{1};
		bzd::impl::CircularList<DummyElement> list;

		list.insert(&a);
		//list.insert<3>(&b);
		const auto result = list.remove(&a);
		EXPECT_TRUE(result);
	}

	{
		DummyElement a{1};
		DummyElement b{1};
		bzd::impl::CircularList<DummyElement> list;

		list.insert(&a);
		//list.insert<4>(&b);
		const auto result = list.remove(&a);
		EXPECT_TRUE(result);
	}

	{
		DummyElement a{1};
		DummyElement b{1};
		bzd::impl::CircularList<DummyElement> list;

		list.insert(&a);
		//list.insert<5>(&b);
		const auto result = list.remove(&a);
		EXPECT_TRUE(result);
	}
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
			std::cout << "Trying to insert " << element.value_ << std::endl;
			const auto result = list.insert(&element);
			if (!result)
			{
				ASSERT_TRUE(result.error() == bzd::impl::ListErrorType::elementAlreadyInserted);
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
			const auto index = rand() % nbElements;
			auto& element = elements[index];

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
	std::thread worker2(workloadInsert);
	//std::thread worker3(workloadInsert);
	//std::thread worker4(workloadInsert);

	worker1.join();
	worker2.join();
	//worker3.join();
	//worker4.join();

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
