#include "bzd/container/impl/non_owning_list.h"

class Mutex
{
public:
	Mutex() = default;

	auto lock()
	{
		return bzd::makePromise(
			[this](bzd::interface::Promise& promise) mutable -> bzd::PromiseReturnType<> {
				bzd::BoolType expected{false};
				if (acquired_.compareExchange(expected, true))
				{
					return bzd::nullresult;
				}
				// Set the promise to the chain list here
				bzd::ignore = promise.pop();
				bzd::ignore = list_.pushFront(promise);
				// std::cout << "Lock " << &promise << ", size: " << list_.size() << std::endl;
				return bzd::nullopt;
			},
			/*isEvent*/ true);
	}

	void unlock()
	{
		acquired_.store(false);
		auto element = list_.back();
		if (element)
		{
			auto& promise = *element;
			// std::cout << "Unlock " << &promise << ", size: " << list_.size() << std::endl;

			promise.poll();
			bzd::ignore = list_.pop(promise);
			// Move promise back to its task
			// bzd::ignore = promise.task_->promises_.pushFront(promise);
			promise.task_->setActive();
		}
	}

private:
	bzd::Atomic<bzd::BoolType> acquired_{false};
	bzd::NonOwningList<bzd::interface::Promise> list_;
};
