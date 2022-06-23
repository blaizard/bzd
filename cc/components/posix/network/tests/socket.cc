#include "cc/components/posix/network/socket.hh"

#include "cc/bzd/test/test.hh"

TEST(Socket, constructor)
{
	const auto maybeSocket = bzd::platform::posix::network::Socket::make(
		bzd::platform::posix::network::AddressFamily::ipV4,
		bzd::platform::posix::network::SocketType::datagram | bzd::platform::posix::network::SocketTypeOption::closeOnExec,
		0);
}
