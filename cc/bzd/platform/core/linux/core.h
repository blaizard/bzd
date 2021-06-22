#pragma once

#include "cc/bzd/container/result.h"
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

	Result<void, Error> stop() noexcept final
	{
		{
			const auto result = pthread_join(thread_, nullptr);
			if (result != 0)
			{
				return bzd::makeError(Error::OTHER);
			}
		}

		{
			const auto result = pthread_attr_destroy(&attr_);
			if (result != 0)
			{
				return bzd::makeError(Error::OTHER);
			}
		}

		return bzd::nullresult;
	}

	bzd::Result<void, Error> start(Callable workload) noexcept final
	{
		const auto nbCores = sysconf(_SC_NPROCESSORS_ONLN);
		if (id_ >= nbCores)
		{
			return bzd::makeError(Error::OTHER);
		}

		stack_.taint();

		if (pthread_attr_init(&attr_) == -1)
		{
			return bzd::makeError(Error::OTHER);
		}

		{
			const auto result = pthread_attr_setstack(&attr_, stack_.data(), stack_.size());
			if (result != 0)
			{
				return bzd::makeError(Error::OTHER);
			}
		}

		workload_ = workload;

		{
			const auto result = pthread_create(&thread_, &attr_, &workloadWrapper, this);
			if (result != 0)
			{
				return bzd::makeError(Error::OTHER);
			}
		}

		return bzd::nullresult;
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
	pthread_attr_t attr_;
	pthread_t thread_;
};

} // namespace bzd::platform::core
