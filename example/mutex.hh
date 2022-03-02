#pragma once

#include "cc/bzd/container/threadsafe/non_owning_forward_list.hh"
/*
class Mutex
{
public:
	Mutex() = default;

	auto lock()
	{
		return bzd::makePromise([this](bzd::interface::Promise& promise, bzd::AnyReference&) mutable -> bzd::Promise<>::ReturnType {
			bzd::BoolType expected{false};
			if (acquired_.compareExchange(expected, true))
			{
				return bzd::nullresult;
			}

			// std::cout << "Lock " << &promise << ", size: " << list_.size() << std::endl;
			promise.setPending(list_);
			return bzd::nullopt;
		});
	}

	void unlock()
	{
		acquired_.store(false);
		auto element = list_.back();
		if (element)
		{
			auto& promise = element.valueMutable();
			// std::cout << "Unlock " << &promise << ", size: " << list_.size() << std::endl;

			promise.setActive(list_);
			// Acquire the lock. this ensures that this specific promise gets.
			promise.poll();
		}
	}

private:
	bzd::Atomic<bzd::BoolType> acquired_{false};
	bzd::threadsafe::NonOwningForwardList<bzd::interface::Promise> list_{};
};
*/
