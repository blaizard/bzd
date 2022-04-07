#pragma once

#include "cc/bzd/container/string.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/channel.hh"
#include "cc/bzd/core/error.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/utility/format/integral.hh"
#include "cc/bzd/utility/scope_guard.hh"
#include "cc/components/posix/network/interface.hh"

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

namespace bzd::platform::posix::network {
// TCP connection
class TCP
//	: public bzd::platform::posix::network::adapter::TCP<TCP>
//	: public bzd::IOStream
{
public:
	TCP() {}

	bzd::Async<> connect(const StringView hostname, const PortType port)
	{
		if (socket_ != -1)
		{
			co_return bzd::error(ErrorType::failure, "A socket is already opened."_csv);
		}

		struct addrinfo hints
		{
		};
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		String<8> portStr;
		toString(portStr, port);

		struct addrinfo* addrs{nullptr};
		if (const auto result = ::getaddrinfo(hostname.data(), portStr.data(), &hints, &addrs); result != 0)
		{
			co_return bzd::error(ErrorType::failure, "getaddrinfo for {}:{}"_csv, hostname, portStr);
		}

		bzd::ScopeGuard scope{[&addrs]() { ::freeaddrinfo(addrs); }};

		for (auto addr = addrs; addr != NULL; addr = addr->ai_next)
		{
			socket_ = ::socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
			if (socket_ == -1)
			{
				continue;
			}

			if (::connect(socket_, addr->ai_addr, addr->ai_addrlen) == 0)
			{
				break;
			}

			::close(socket_);
			socket_ = -1;
		}

		if (socket_ == -1)
		{
			co_return bzd::error(ErrorType::failure, "Cannot resolve {}, errno {}"_csv, hostname, errno);
		}

		co_return {};
	}

	void disconnect()
	{
		if (socket_ != -1)
		{
			::close(socket_);
			socket_ = -1;
		}
	}

	/*
		bzd::Async<SizeType> write(const bzd::Span<const T> data) noexcept override
		{
			::write();
		}
	*/
	~TCP() { disconnect(); }

private:
	int socket_{-1};
};
} // namespace bzd::platform::posix::network
