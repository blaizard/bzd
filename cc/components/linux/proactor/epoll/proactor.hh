#pragma once

#include "cc/components/posix/error.hh"
#include "cc/components/posix/proactor/interface.hh"
#include "cc/components/posix/proactor/sync/proactor.hh"

#include <cerrno>
#include <cstring>
#include <sys/epoll.h>
#include <unistd.h>

namespace bzd::platform::linux::epoll {

/// Proactor for basic POSIX functionalities
class Proactor
	: posix::sync::Proactor
	, public bzd::platform::posix::Proactor<Proactor>
{
private:
	struct EpollData
	{
		const posix::FileDescriptor fd_;
		bzd::async::Executable* executable_{nullptr};
	};

public:
	using posix::sync::Proactor::connect;
	using posix::sync::Proactor::write;

public:
	// NOLINTNEXTLINE(bugprone-exception-escape)
	bzd::Async<> init() noexcept
	{
		if (epollFd_.isValid())
		{
			co_return bzd::error::Failure("Already initialized"_csv);
		}
		epollFd_ = ::epoll_create1(EPOLL_CLOEXEC);
		if (!epollFd_.isValid())
		{
			co_return bzd::error::Errno("epoll_create1");
		}
		co_return {};
	}

	/// Perform an asynchronous read operator.
	// NOLINTNEXTLINE(bugprone-exception-escape)
	bzd::Async<bzd::Span<bzd::Byte>> read(const posix::FileDescriptor fd, const bzd::Span<bzd::Byte> data) noexcept
	{
		// Wait until some data is available.
		{
			EpollData epollData{fd};
			::epoll_event ev{.events = EPOLLIN, .data{.ptr = &epollData}};

			// 1. Register event
			co_await bzd::async::wait([&](auto& executable) {
				epollData.executable_ = &executable;
				::epoll_ctl(epollFd_.native(), EPOLL_CTL_ADD, epollData.fd_.native(), &ev);
			});
		}

		// TODO: create a scope to opt-out as well.

		// 2. read
		const auto size = ::read(fd.native(), data.data(), data.size());
		if (size < 0)
		{
			co_return bzd::error::Errno("read");
		}
		co_return data.subSpan(0, static_cast<bzd::Size>(size));
	}

	/// Execution loop for the reactor to poll events.
	// NOLINTNEXTLINE(bugprone-exception-escape)
	bzd::Async<> exec() noexcept
	{
		const auto& executor = co_await bzd::async::getExecutor();
		while (executor.isRunning())
		{
			bzd::Array<::epoll_event, 10U> events;
			const int count = ::epoll_wait(epollFd_.native(), events.data(), events.size(), /*timeout ms, 0=return immediately*/ 1);
			if (count == -1)
			{
				co_return bzd::error::Errno("epoll_wait");
			}
			for (int i = 0; i < count; ++i)
			{
				auto& data{*reinterpret_cast<EpollData*>(events[i].data.ptr)};
				::epoll_ctl(epollFd_.native(), EPOLL_CTL_DEL, data.fd_.native(), nullptr);
				data.executable_->schedule();
			}
			co_await bzd::async::yield();
		}
		co_return {};
	}

private:
	posix::FileDescriptorOwner epollFd_{};
};

} // namespace bzd::platform::linux::epoll
