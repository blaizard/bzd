#pragma once

namespace bzd::platform::posix::network {
// Socket address information.
template <class CRTP>
class Socket
{
public:
	static auto domain() { return CRTP::domain(); }
	struct ::sockaddr address();
	::socklen_t size();
};
} // namespace bzd::platform::posix::network
