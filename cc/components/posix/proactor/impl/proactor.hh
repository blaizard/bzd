#pragma once

#include "cc/components/posix/error.hh"
#include "cc/components/posix/proactor/interface.hh"

#include <cerrno>
#include <cstring>
#include <sys/epoll.h>
#include <unistd.h>

namespace bzd::platform::posix::impl {

/// Proactor for basic POSIX functionalities
/// This is a mock of a proactor from a reactor design, for compatibility with POSIX.
class Proactor : public bzd::platform::posix::Proactor<Proactor>
{
private:
	struct EpollData
	{
		const FileDescriptor fd_;
		bzd::async::Executable* executable_{nullptr};
	};

public:
	bzd::Async<> init() noexcept
	{
		if (epollFd_.isValid())
		{
			co_return bzd::error::Failure("Already initialized"_csv);
		}
		epollFd_ = ::epoll_create1(/*size is ignored but must be set*/ 0);
		if (!epollFd_.isValid())
		{
			co_return bzd::error::Posix("epoll_create1");
		}
		co_return {};
	}

	/// Perform an asynchronous read operator.
	bzd::Async<bzd::Span<bzd::Byte>> read(const FileDescriptor fd, const bzd::Span<bzd::Byte> data) noexcept
	{
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
			co_return bzd::error::Posix("read");
		}
		co_return data.subSpan(0, static_cast<bzd::Size>(size));
	}

	/// Perform an asynchronous read operator.
	bzd::Async<bzd::Size> write(const FileDescriptor fd, const bzd::Span<const bzd::Byte> data) noexcept
	{
		bzd::Size size{0u};
		while (size < data.size())
		{
			const auto result = ::write(fd.native(), &data.at(size), data.size() - size);
			if (result < 0)
			{
				co_return bzd::error::Posix("write");
			}
			size += result;
		}
		co_return size;
	}

	/// Execution loop for the reactor to poll events.
	bzd::Async<> exec() noexcept
	{
		const auto& executor = co_await bzd::async::getExecutor();
		while (executor.isRunning())
		{
			bzd::Array<::epoll_event, 10U> events;
			const int count = ::epoll_wait(epollFd_.native(), events.data(), events.size(), /*timeout ms*/ 1);
			if (count == -1)
			{
				co_return bzd::error::Posix("epoll_wait");
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
	FileDescriptorOwner epollFd_{};
};

} // namespace bzd::platform::posix::impl
