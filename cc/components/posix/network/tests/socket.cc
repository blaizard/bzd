#include "cc/components/posix/network/socket.hh"

#include "cc/bzd/test/test.hh"

namespace bzd::platform::posix::network {

TEST(Socket, constructor)
{
	const auto maybeSocket = Socket::make(AddressFamily::ipV4, SocketType::datagram | SocketTypeOption::closeOnExec, 0);
}

} // namespace bzd::platform::posix::network
