#pragma once

#include "cc/bzd/container/optional.h"
#include "cc/bzd/platform/core.h"
#include "cc/bzd/platform/core/stack.h"

#include <iostream>
#include <pthread.h>

namespace bzd::platform::core {

template <SizeType StackSize>
class Linux : public bzd::platform::Core
{
private:
	using Self = Linux<StackSize>;

public:
	explicit Linux() = default;

	void init() noexcept final {}

	void run(WorkloadType workload) noexcept final
	{
		stack_.taint();

		pthread_attr_t attr_;
		pthread_t thread_;

		if (pthread_attr_init(&attr_) == -1)
		{
			return;
		}

		{
			const auto result = pthread_attr_setstack(&attr_, stack_.data(), stack_.size());
			if (result != 0)
			{
				return;
			}
		}

		workload_ = workload;

		{
			const auto result = pthread_create(&thread_, &attr_, &workloadWrapper, this);
			if (result != 0)
			{
				return;
			}
		}

		// Workload should be running here

		{
			const auto result = pthread_join(thread_, nullptr);
			if (result != 0)
			{
				return;
			}
		}

		{
			const auto result = pthread_attr_destroy(&attr_);
			if (result != 0)
			{
				return;
			}
		}
	}

	SizeType getStackUsage() noexcept final { return stack_.estimateMaxUsage(); }

private:
	static void* workloadWrapper(void* object)
	{
		auto linux = reinterpret_cast<Self*>(object);
		auto& workload = linux->workload_.value();
		std::cout << "Workload Wrapper Enter" << std::endl;
		workload();
		std::cout << "Workload Wrapper Exit" << std::endl;
		return nullptr;
	}

private:
	Stack<StackSize> stack_{};
	bzd::Optional<WorkloadType> workload_;
};

} // namespace bzd::platform::core
