#include "bzd/container/impl/non_owning_list.h"

#include "bzd/test/sync_point.h"
#include "bzd/utility/ignore.h"
#include "cc_test/test.h"

#include <iostream>
#include <mutex>
#include <set>
#include <thread>
#include <vector>
class DummyElement : public bzd::impl::ListElement<bzd::impl::ListElementMultiContainer>
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
	bzd::impl::NonOwningList<DummyElement> list;
	bzd::ignore = list.insert(&a);
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

TEST(NonOwningList, insertWhileInsert)
{
	insertWhileInsertDoWork<bzd::test::InjectPoint0>();
	insertWhileInsertDoWork<bzd::test::InjectPoint1>();
	insertWhileInsertDoWork<bzd::test::InjectPoint2>();
	insertWhileInsertDoWork<bzd::test::InjectPoint3>();
	insertWhileInsertDoWork<bzd::test::InjectPoint4>();
}

template <class T>
void removeWhileInsertDoWork()
{
	DummyElement a{1};
	DummyElement b{2};
	bzd::impl::NonOwningList<DummyElement> list;
	bzd::ignore = list.insert(&a);
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

TEST(NonOwningList, removeWhileInsert)
{
	removeWhileInsertDoWork<bzd::test::InjectPoint0>();
	removeWhileInsertDoWork<bzd::test::InjectPoint1>();
	removeWhileInsertDoWork<bzd::test::InjectPoint2>();
	removeWhileInsertDoWork<bzd::test::InjectPoint3>();
	removeWhileInsertDoWork<bzd::test::InjectPoint4>();
}

template <class T>
void insertWhileRemoveDoWork()
{
	DummyElement a{1};
	DummyElement b{2};
	bzd::impl::NonOwningList<DummyElement> list;
	bzd::ignore = list.insert(&a);
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

TEST(NonOwningList, insertWhileRemove)
{
	insertWhileRemoveDoWork<bzd::test::InjectPoint0>();
	insertWhileRemoveDoWork<bzd::test::InjectPoint1>();
	insertWhileRemoveDoWork<bzd::test::InjectPoint2>();
	insertWhileRemoveDoWork<bzd::test::InjectPoint3>();
	insertWhileRemoveDoWork<bzd::test::InjectPoint4>();
}

template <class T>
void removeWhileRemoveLeftDoWork()
{
	DummyElement a{1};
	DummyElement b{2};
	bzd::impl::NonOwningList<DummyElement> list;
	bzd::ignore = list.insert(&a);
	bzd::ignore = list.insert(&b);
	using SyncPoint = bzd::test::SyncPoint<struct concurrency>;

	std::thread workerRemove1([&list, &b]() {
		typename SyncPoint::template Type<2>();
		const auto result = list.remove(&b);
		EXPECT_TRUE(result);
		typename SyncPoint::template Type<3>();
	});
	std::thread workerRemove2([&list, &a]() {
		const auto result = list.remove<T, typename SyncPoint::template Type<1>, typename SyncPoint::template Type<4>>(&a);
		EXPECT_TRUE(result);
	});

	workerRemove1.join();
	workerRemove2.join();

	const auto result = list.sanityCheck([](const auto&) -> bool { return true; });
	EXPECT_TRUE(result);
	EXPECT_EQ(*result, 0);
	EXPECT_EQ(list.size(), 0);
}

TEST(NonOwningList, removeWhileRemoveLeft)
{
	removeWhileRemoveLeftDoWork<bzd::test::InjectPoint0>();
	removeWhileRemoveLeftDoWork<bzd::test::InjectPoint1>();
	removeWhileRemoveLeftDoWork<bzd::test::InjectPoint2>();
	removeWhileRemoveLeftDoWork<bzd::test::InjectPoint3>();
	removeWhileRemoveLeftDoWork<bzd::test::InjectPoint4>();
}

template <class T>
void removeWhileRemoveRightDoWork()
{
	DummyElement a{1};
	DummyElement b{2};
	bzd::impl::NonOwningList<DummyElement> list;
	bzd::ignore = list.insert(&a);
	bzd::ignore = list.insert(&b);
	using SyncPoint = bzd::test::SyncPoint<struct concurrency>;

	std::thread workerRemove1([&list, &a]() {
		typename SyncPoint::template Type<2>();
		const auto result = list.remove(&a);
		EXPECT_TRUE(result);
		typename SyncPoint::template Type<3>();
	});
	std::thread workerRemove2([&list, &b]() {
		const auto result = list.remove<T, typename SyncPoint::template Type<1>, typename SyncPoint::template Type<4>>(&b);
		EXPECT_TRUE(result);
	});

	workerRemove1.join();
	workerRemove2.join();

	const auto result = list.sanityCheck([](const auto&) -> bool { return true; });
	EXPECT_TRUE(result);
	EXPECT_EQ(*result, 0);
	EXPECT_EQ(list.size(), 0);
}

TEST(NonOwningList, removeWhileRemoveRight)
{
	removeWhileRemoveRightDoWork<bzd::test::InjectPoint0>();
	removeWhileRemoveRightDoWork<bzd::test::InjectPoint1>();
	removeWhileRemoveRightDoWork<bzd::test::InjectPoint2>();
	removeWhileRemoveRightDoWork<bzd::test::InjectPoint3>();
	removeWhileRemoveRightDoWork<bzd::test::InjectPoint4>();
	removeWhileRemoveRightDoWork<bzd::test::InjectPoint5>();
	removeWhileRemoveRightDoWork<bzd::test::InjectPoint6>();
}

uint64_t getTimestampMs()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

TEST(NonOwningList, insertionStress)
{
	srand(time(NULL));

	constexpr bzd::SizeType nbIterations = 100000;
	constexpr bzd::SizeType nbElements = 10;
	constexpr uint64_t timeMaxMs = 1000 * 1000;
	const auto timeStart = getTimestampMs();

	struct Data
	{
		bzd::impl::NonOwningList<DummyElement> list{};
		bzd::Atomic<bzd::UInt16Type> inserted[nbElements]{};
		bzd::Atomic<bzd::SizeType> insertion{0};
		bzd::Atomic<bzd::SizeType> removal{0};

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
	Data data2{};

	// Elements
	static std::vector<DummyElement> elements;
	for (bzd::SizeType i = 0; i < nbElements; ++i)
	{
		elements.push_back(DummyElement{i});
	}
	ASSERT_EQ(nbElements, elements.size());

	const auto workloadInsert = [&timeStart](Data* pData) {
		int counter = nbIterations;
		while (--counter && getTimestampMs() - timeStart < timeMaxMs)
		{
			// std::this_thread::yield();
			auto& element = elements[rand() % nbElements];

			const auto result = pData->list.insert(&element);
			if (!result)
			{
				ASSERT_EQ(result.error(), bzd::impl::ListErrorType::elementAlreadyInserted);
			}
			else
			{
				++(pData->inserted[element.value_]);
				++pData->insertion;
			}
		}
	};

	const auto workloadRemove = [&timeStart](Data* pData) {
		int counter = nbIterations;
		while (--counter && getTimestampMs() - timeStart < timeMaxMs)
		{
			// std::this_thread::yield();
			auto& element = elements[rand() % nbElements];

			const auto result = pData->list.remove(&element);
			if (!result)
			{
				ASSERT_TRUE(result.error() == bzd::impl::ListErrorType::elementAlreadyRemoved ||
							result.error() == bzd::impl::ListErrorType::notFound);
			}
			else
			{
				--(pData->inserted[element.value_]);
				++pData->removal;
			}
		}
	};

	bzd::ignore = workloadRemove;

	std::thread worker1(workloadInsert, &data1);
	std::thread worker2(workloadInsert, &data1);
	std::thread worker3(workloadRemove, &data1);
	std::thread worker4(workloadRemove, &data1);
	std::thread worker5(workloadInsert, &data2);
	std::thread worker6(workloadInsert, &data2);
	std::thread worker7(workloadRemove, &data2);
	std::thread worker8(workloadRemove, &data2);

	worker1.join();
	worker2.join();
	worker3.join();
	worker4.join();
	worker5.join();
	worker6.join();
	worker7.join();
	worker8.join();

	for (bzd::SizeType i = 0; i < nbElements; ++i)
	{
		std::cout << i << ": ";
		data1.list.printNode(&elements[i]);
	}
	std::cout << std::endl;

	EXPECT_TRUE(getTimestampMs() - timeStart < timeMaxMs);

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
