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
#include <stdlib.h>

namespace bzd::platform::linux {

template <Size stackSize>
class Core : public bzd::platform::Core
{
private:
	using Self = Core<stackSize>;

public:
	explicit Core(const CoreId core_id, const float) noexcept : core_id_{core_id} {}

	~Core() noexcept
	{
		if (stack_.hasValue())
		{
			::std::cout << "Stack usage: " << getStackUsage() << " / " << stackSize << ::std::endl;
			::free(stack_->data());
		}
	}

	Result<void, bzd::Error> start(const bzd::FunctionRef<void(bzd::platform::Core&)> workload) noexcept override
	{
		// Allocate the aligned memory.
		if (stack_.empty())
		{
			const auto alignment = ::sysconf(_SC_PAGESIZE);
			if (alignment == -1)
			{
				return bzd::error::Errno("sysconf");
			}

			void* memory;
			if (const auto result = ::posix_memalign(&memory, alignment, stackSize); result != 0)
			{
				return bzd::error::Errno("sysconf", result);
			}
			stack_.emplace(Span<Byte>{static_cast<Byte*>(memory), stackSize});

			// Taint the stack to montior its usage.
			stack_->taint();
		}

		if (::pthread_attr_init(&attr_) != 0)
		{
			return bzd::error::Errno("pthread_attr_init");
		}

		if (::pthread_attr_setstack(&attr_, stack_->data(), stack_->size()) != 0)
		{
			return bzd::error::Errno("pthread_attr_setstack");
		}

		workload_ = workload;

		if (::pthread_create(&thread_, &attr_, &workloadWrapper, this) != 0)
		{
			return bzd::error::Errno("pthread_create");
		}

		return bzd::nullresult;
	}

	Result<void, bzd::Error> stop() noexcept override
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

	StackSize getStackUsage() noexcept override { return stack_->estimateMaxUsage(); }

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

	CoreId getId() noexcept override { return core_id_; }

private:
	static void* workloadWrapper(void* object)
	{
		auto linux = reinterpret_cast<Self*>(object);
		auto& workload = linux->workload_.value();
		::std::cout << "Workload Wrapper Enter" << ::std::endl;
		workload(*linux);
		::std::cout << "Workload Wrapper Exit" << ::std::endl;
		return nullptr;
	}

private:
	CoreId core_id_;
	bzd::Optional<bzd::interface::Stack<StackDirection::downward>> stack_{};
	bzd::Optional<bzd::FunctionRef<void(bzd::platform::Core&)>> workload_;
	pthread_attr_t attr_;
	pthread_t thread_;
};

} // namespace bzd::platform::linux
