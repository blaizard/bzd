#pragma once

#include "cc/components/posix/error.hh"
#include "cc/components/posix/proactor/interface.hh"
#include "cc/components/posix/proactor/sync/proactor.hh"

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
		bzd::async::ExecutableRef executable_{};
	};

public:
	using posix::sync::Proactor::connect;
	using posix::sync::Proactor::write;

public:
	bzd::Async<> init() noexcept;

	/// Perform an asynchronous read operator.
	bzd::Async<bzd::Span<bzd::Byte>> read(const posix::FileDescriptor fd, const bzd::Span<bzd::Byte> data) noexcept;

	/// Execution loop for the reactor to poll events.
	bzd::Async<> exec() noexcept;

private:
	posix::FileDescriptorOwner epollFd_{};
};

} // namespace bzd::platform::linux::epoll
