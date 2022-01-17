#pragma once

#include "cc/bzd/container/string.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/error.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/utility/format/integral.hh"
#include "cc/bzd/utility/scope_guard.hh"
#include "cc/components/posix/network/interface.hh"

namespace bzd::network {
// TCP connection
class TCP
	: public bzd::platform::posix::network::adapter::TCP<TCP>
	, public bzd::IOStream
{
public:
	TCP() {}

	Async<void, Error> connect(const StringView hostname, const PortType port)
	{
		::addrinfo hints{};
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;

		String<8> portStr;
		toString(portStr, port);

		::addinfo* addrs{nullptr};
		if (const auto result = ::getaddrinfo(hostname.data(), portStr.data(), &hints, &addrs); result != 0)
		{
			co_return bzd::error(Error::failure, CSTR("getaddrinfo for {}:{}"), hostname, portStr);
		}

		bzd::ScopeGuard scope{[&addrs]() { ::freeaddrinfo(addrs); }};

		for (auto addr = addrs; addr != NULL; addr = addr->ai_next)
		{
			sd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
			if (sd == -1)
			{
				err = errno;
				break; // if using AF_UNSPEC above instead of AF_INET/6 specifically,
					   // replace this 'break' with 'continue' instead, as the 'ai_family'
					   // may be different on the next iteration...
			}

			if (connect(sd, addr->ai_addr, addr->ai_addrlen) == 0) break;

			err = errno;

			close(sd);
			sd = -1;
		}

		freeaddrinfo(addrs);

		if (sd == -1)
		{
			fprintf(stderr, "%s: %s\n", hostname, strerror(err));
			abort();
		}
	}

	~TCP()
	{
		// close
	}
};
} // namespace bzd::network
