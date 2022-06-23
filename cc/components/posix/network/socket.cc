#include "cc/components/posix/network/socket.hh"

#include "cc/components/posix/error.hh"

#include <unistd.h>

namespace bzd::platform::posix::network {

bzd::Result<Socket, bzd::Error> Socket::make(const AddressFamily family, const SocketType type, const int protocol) noexcept
{
	const auto fd = ::socket(bzd::toUnderlying(family), bzd::toUnderlying(type), protocol);
	if (fd == -1)
	{
		return bzd::error::Posix("socket");
	}
	return bzd::move(Socket{fd});
}

Socket::~Socket() noexcept
{
	if (fd_ != -1)
	{
		::close(fd_);
		fd_ = -1;
	}
}
} // namespace bzd::platform::posix::network
