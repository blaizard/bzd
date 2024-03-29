#include "cc/components/posix/network/socket.hh"

#include "cc/components/posix/error.hh"

#include <unistd.h>

namespace bzd::components::posix::network {

bzd::Result<Socket, bzd::Error> Socket::make(const AddressFamily family, const SocketType type, const int protocol) noexcept
{
	const auto fd = ::socket(bzd::toUnderlying(family), bzd::toUnderlying(type), protocol);
	if (fd == -1)
	{
		return bzd::error::Errno("socket");
	}

	const int yes = 1;
	if (::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) != 0)
	{
		return bzd::error::Errno("setsockopt");
	}

	return bzd::move(Socket{fd});
}

bzd::Result<Socket, bzd::Error> Socket::make(const Address& address, const SocketTypeOption options) noexcept
{
	return make(address.family(), address.socketType() | options, address.protocol());
}

bzd::Result<void, bzd::Error> Socket::bind(const Address& address) noexcept
{
	const auto result = ::bind(fd_.native(), const_cast<::sockaddr*>(address.native()), address.size());
	if (result != 0)
	{
		return bzd::error::Errno("bind");
	}
	return bzd::nullresult;
}

bzd::Result<void, bzd::Error> Socket::listen(const bzd::Size maxPendingConnection) noexcept
{
	const auto result = ::listen(fd_.native(), static_cast<int>(maxPendingConnection));
	if (result != 0)
	{
		return bzd::error::Errno("listen");
	}
	return bzd::nullresult;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
bzd::Async<Socket> Socket::accept() noexcept
{
	::sockaddr_storage addr;
	::socklen_t size{sizeof(addr)};
	const auto fdPeer = ::accept(fd_.native(), reinterpret_cast<::sockaddr*>(&addr), &size);
	if (fdPeer == -1)
	{
		co_return bzd::error::Errno("accept");
	}
	co_return Socket{fdPeer};
}

void Socket::reset() noexcept { fd_.reset(); }

} // namespace bzd::components::posix::network
