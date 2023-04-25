#pragma once

#include "cc/components/posix/error.hh"
#include "cc/components/posix/proactor/interface.hh"
#include "cc/components/posix/proactor/sync/proactor.hh"

namespace bzd::components::linux::epoll {

/// Proactor for basic POSIX functionalities
class Proactor
	: posix::sync::Proactor
	, public bzd::components::posix::Proactor<Proactor>
{
private:
	struct EpollData
	{
		const posix::FileDescriptor fd_;
		bzd::async::ExecutableSuspended executable_{};
	};

public:
	using posix::sync::Proactor::connect;
	using posix::sync::Proactor::write;

	template <class Context>
	constexpr Proactor(Context&) noexcept
	{
	}

public:
	bzd::Async<> init() noexcept;

	/// Perform an asynchronous read operator.
	bzd::Async<bzd::Span<const bzd::Byte>> read(const posix::FileDescriptor fd, bzd::Span<bzd::Byte>&& data) noexcept;

	/// Execution loop for the reactor to poll events.
	bzd::Async<> exec() noexcept;

private:
	posix::FileDescriptorOwner epollFd_{};
};

} // namespace bzd::components::linux::epoll
