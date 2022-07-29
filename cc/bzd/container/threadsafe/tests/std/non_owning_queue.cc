#include "cc/bzd/container/threadsafe/non_owning_queue.hh"

#include "cc/bzd/test/test.hh"

#include <iostream>
#include <mutex>
#include <set>
#include <thread>
#include <vector>

namespace bzd::test {

struct ListElement : public bzd::threadsafe::NonOwningQueueElement
{
	ListElement() = default;
	ListElement(bzd::Size v) : value{v} {}
	bzd::Size value{0};
};

/*
TEST(NonOwningQueue, Stress)
{
	srand(time(NULL));

	// Elements
	static std::vector<bzd::test::ListElement> elements;
	for (bzd::Size i = 0; i < 1000; ++i)
	{
		elements.push_back(bzd::test::ListElement{i});
	}

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
}
*/
} // namespace bzd::test
