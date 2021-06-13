#pragma once

#include "cc/bzd/container/optional.h"
#include "cc/bzd/platform/core/linux/linux.h"
#include "cc/bzd/platform/core/stack.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE // Needed for sched_setaffinity
#endif
#include <iostream>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>

namespace bzd::platform::core {

template <SizeType N>
class Linux : public bzd::platform::core::impl::Linux
{
private:
	using Self = Linux<N>;

public:
	explicit Linux(const CoreId coreId) noexcept : id_{coreId} {}

	void init() noexcept final
	{
		const auto nbCores = sysconf(_SC_NPROCESSORS_ONLN);
		if (id_ >= nbCores)
		{
			return;
		}
	}

	void run(Callable workload) noexcept final
	{
		stack_.taint();

		pthread_attr_t attr;
		pthread_t thread;

		if (pthread_attr_init(&attr) == -1)
		{
			return;
		}

		{
			const auto result = pthread_attr_setstack(&attr, stack_.data(), stack_.size());
			if (result != 0)
			{
				return;
			}
		}

		workload_ = workload;

		{
			const auto result = pthread_create(&thread, &attr, &workloadWrapper, this);
			if (result != 0)
			{
				return;
			}
		}

		// Workload should be running here

		{
			const auto result = pthread_join(thread, nullptr);
			if (result != 0)
			{
				return;
			}
		}

		{
			const auto result = pthread_attr_destroy(&attr);
			if (result != 0)
			{
				return;
			}
		}
	}

	StackSize getStackUsage() noexcept final { return stack_.estimateMaxUsage(); }

	CoreId getId() noexcept final { return id_; }

private:
	static void* workloadWrapper(void* object)
	{
		auto linux = reinterpret_cast<Self*>(object);

		// Set CPU affinity
		{
			cpu_set_t cpuset;
			CPU_ZERO(&cpuset);
			CPU_SET(linux->id_, &cpuset);
			sched_setaffinity(/*current thread*/ 0, sizeof(cpu_set_t), &cpuset);
		}

		auto& workload = linux->workload_.value();
		std::cout << "Workload Wrapper Enter" << std::endl;
		workload();
		std::cout << "Workload Wrapper Exit" << std::endl;
		return nullptr;
	}

private:
	Stack<N> stack_{};
	bzd::Optional<Callable> workload_;
	CoreId id_;
};

} // namespace bzd::platform::core
