#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/result.hh"
#include "cc/bzd/container/stack.hh"
#include "cc/bzd/core/error.hh"
#include "cc/components/linux/core/interface.hh"
#include "cc/components/posix/error.hh"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE // Needed for sched_setaffinity
#endif
#include <iostream>
#include <pthread.h>
#include <sched.h>
#include <unistd.h>

namespace bzd::platform::linux {

template <Size N>
class Core : public bzd::platform::Core<Core<N>>
{
private:
	using Self = Core<N>;
	using Parent = bzd::platform::Core<Self>;

public:
	explicit Core(const CoreId, const float) noexcept {}

	~Core() noexcept { ::std::cout << "Stack usage: " << getStackUsage() << " / " << N << ::std::endl; }

	Result<void, bzd::Error> start(const bzd::platform::WorkloadType& workload) noexcept
	{
		stack_.taint();

		if (::pthread_attr_init(&attr_) != 0)
		{
			return bzd::error::Errno("pthread_attr_init");
		}

		{
			const auto result = ::pthread_attr_setstack(&attr_, stack_.data(), stack_.size());
			if (result != 0)
			{
				return bzd::error::Errno("pthread_attr_setstack");
			}
		}

		workload_ = workload;

		{
			const auto result = ::pthread_create(&thread_, &attr_, &workloadWrapper, this);
			if (result != 0)
			{
				return bzd::error::Errno("pthread_create");
			}
		}

		return bzd::nullresult;
	}

	Result<void, bzd::Error> stop() noexcept
	{
		{
			const auto result = ::pthread_join(thread_, nullptr);
			if (result != 0)
			{
				return bzd::error::Errno("pthread_join");
			}
		}

		{
			const auto result = ::pthread_attr_destroy(&attr_);
			if (result != 0)
			{
				return bzd::error::Errno("pthread_attr_destroy");
			}
		}

		return bzd::nullresult;
	}

	StackSize getStackUsage() noexcept { return stack_.estimateMaxUsage(); }

	bzd::Size startUsageMonitoring() noexcept
	{
		// We can use https://man7.org/linux/man-pages/man2/getrusage.2.html

		// Get the clock_id associated to the current thread.
		clockid_t clockId;
		::pthread_getcpuclockid(thread_, &clockId);

		struct timespec ts;
		if (::clock_gettime(clockId, &ts))
		{
		}

		[[maybe_unused]] const auto timeNano = ts.tv_sec * 1000000000 + ts.tv_nsec;

		// in percent
		// use pthread_getcpuclockid
		return 0;
	}

	void getUsage() noexcept {}

	CoreId getId() noexcept { return 0; }

private:
	static void* workloadWrapper(void* object)
	{
		auto linux = reinterpret_cast<Self*>(object);
		auto& workload = linux->workload_.value();
		::std::cout << "Workload Wrapper Enter" << ::std::endl;
		workload();
		::std::cout << "Workload Wrapper Exit" << ::std::endl;
		return nullptr;
	}

private:
	bzd::Stack<N> stack_{};
	bzd::Optional<bzd::platform::WorkloadType> workload_;
	pthread_attr_t attr_;
	pthread_t thread_;
};

} // namespace bzd::platform::linux
