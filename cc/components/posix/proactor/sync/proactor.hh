#pragma once

#include "cc/components/posix/error.hh"
#include "cc/components/posix/network/socket_options.hh"
#include "cc/components/posix/proactor/interface.hh"

#include <cerrno>
#include <cstring>
#include <poll.h>
#include <unistd.h>

namespace bzd::platform::posix::sync {

/// Proactor for basic POSIX functionalities
class Proactor : public bzd::platform::posix::Proactor<Proactor>
{
public:
	/// Perform a synchronous write operator.
	// NOLINTNEXTLINE(bugprone-exception-escape)
	bzd::Async<bzd::Size> write(const FileDescriptor fd, const bzd::Span<const bzd::Byte> data) noexcept
	{
		bzd::Size size{0u};
		while (size < data.size())
		{
			const auto result = ::write(fd.native(), &data.at(size), data.size() - size);
			if (result < 0)
			{
				if (errno == EAGAIN)
				{
					co_await bzd::async::yield();
					continue;
				}
				co_return bzd::error::Errno("write");
			}
			size += result;
		}
		co_return size;
	}

	/// Perform a synchronous read operator.
	// NOLINTNEXTLINE(bugprone-exception-escape)
	bzd::Async<bzd::Span<bzd::Byte>> read(const FileDescriptor fd, const bzd::Span<bzd::Byte> data) noexcept
	{
		while (true)
		{
			const auto result = ::read(fd.native(), data.data(), data.size());
			if (result == -1)
			{
				if (errno == EAGAIN || errno == EWOULDBLOCK)
				{
					co_await bzd::async::yield();
					continue;
				}
				co_return bzd::error::Errno("read");
			}
			co_return data.first(result);
		}
	}

	/// Perform a synchronous connect operator.
	// NOLINTNEXTLINE(bugprone-exception-escape)
	bzd::Async<> connect(const FileDescriptor fd, const network::Address& address) noexcept
	{
		const auto result = ::connect(fd.native(), const_cast<::sockaddr*>(address.native()), address.size());
		if (result == -1)
		{
			if (errno == EAGAIN || errno == EINPROGRESS)
			{
				co_await !poll(fd, POLLOUT);
				auto maybeSocketResult = network::getSocketOptions(fd, SOL_SOCKET, SO_ERROR);
				if (!maybeSocketResult)
				{
					co_return bzd::move(maybeSocketResult).propagate();
				}
				if (maybeSocketResult.value() != 0)
				{
					co_return bzd::error::Errno("connect", maybeSocketResult.value());
				}
				co_return {};
			}
			co_return bzd::error::Errno("connect");
		}
		co_return {};
	}

private:
	// NOLINTNEXTLINE(bugprone-exception-escape)
	bzd::Async<> poll(const FileDescriptor fd, const short events) noexcept
	{
		::pollfd pfd{fd.native(), events, 0};
		int result;
		while (true)
		{
			result = ::poll(&pfd, 1, 0);
			if (result == -1)
			{
				co_return bzd::error::Errno("poll");
			}
			if (result > 0)
			{
				break;
			}
			co_await bzd::async::yield();
		}
		if (pfd.revents & POLLERR)
		{
			co_return bzd::error::Failure("POLLERR returned");
		}
		co_return {};
	}
};

} // namespace bzd::platform::posix::sync
