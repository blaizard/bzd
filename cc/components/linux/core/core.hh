#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/result.hh"
#include "cc/bzd/container/stack.hh"
#include "cc/bzd/core/error.hh"
#include "cc/bzd/core/print.hh"
#include "cc/bzd/math/ceil.hh"
#include "cc/bzd/utility/align_up.hh"
#include "cc/components/linux/core/interface.hh"
#include "cc/components/posix/error.hh"
#ifndef _GNU_SOURCE
#define _GNU_SOURCE // Needed for sched_setaffinity
#endif
#include <iostream>
#include <pthread.h>
#include <sched.h>
#include <stdlib.h>
#include <unistd.h>

namespace bzd::platform::linux {

template <class Context>
class Core : public bzd::platform::Core
{
private:
	using Self = Core<Context>;

public:
	explicit Core(const Context) noexcept : core_id_{0}
	{
		bzd::ignore = Context::Config::priority;
		bzd::ignore = Context::Config::name;
	}

	~Core() noexcept
	{
		if (stack_.hasValue())
		{
			::free(stack_->data());
		}
	}

	Result<void, bzd::Error> start(const bzd::FunctionRef<void(bzd::platform::Core&)> workload) noexcept override
	{
		// Allocate the aligned memory for the stack.
		if (stack_.empty())
		{
			const auto alignment = ::sysconf(_SC_PAGESIZE);
			if (alignment == -1)
			{
				return bzd::error::Errno("sysconf");
			}

			const Size actualSize = bzd::alignUp(Context::Config::stackSize, alignment);
			void* memory = nullptr;
			if (const auto result = ::posix_memalign(&memory, alignment, actualSize); result != 0)
			{
				return bzd::error::Errno("sysconf", result);
			}
			stack_.emplace(Span<Byte>{static_cast<Byte*>(memory), actualSize});

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
		if (::pthread_join(thread_, nullptr) != 0)
		{
			return bzd::error::Errno("pthread_join");
		}

		if (::pthread_attr_destroy(&attr_) != 0)
		{
			return bzd::error::Errno("pthread_attr_destroy");
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
		bzd::print("Workload Wrapper Enter\n"_csv).sync();
		workload(*linux);
		bzd::print("Workload Wrapper Exit\n"_csv).sync();
		bzd::print("Stack usage: {:} / {:}\n"_csv, linux->getStackUsage(), linux->stack_->size()).sync();

		return nullptr;
	}

private:
	CoreId core_id_;
	bzd::Optional<bzd::interface::Stack<StackDirection::downward>> stack_ {
	};
	bzd::Optional<bzd::FunctionRef<void(bzd::platform::Core&)>> workload_;
	pthread_attr_t attr_;
	pthread_t thread_;
};

} // namespace bzd::platform::linux
