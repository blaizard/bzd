#include "cc/components/posix/network/socket_options.hh"

#include <sys/socket.h>

namespace bzd::platform::posix::network {

Result<SocketOption, Error> getSocketOptions(const FileDescriptor fd, const SocketOptionLevel level, const SocketOptionName name) noexcept
{
    SocketOption value;
	::socklen_t valueSize{sizeof(value)};
	if (::getsockopt(fd.native(), level, name, &value, &valueSize) == -1)
	{
	    return bzd::error::Errno("getsockopt");
	}
    return value;
}

Result<void, Error> setSocketOptions(const FileDescriptor fd, const SocketOptionLevel level, const SocketOptionName name, const SocketOption value) noexcept
{
	if (::setsockopt(fd.native(), level, name, &value, sizeof(value)) == -1)
	{
	    return bzd::error::Errno("setsockopt");
	}
    return bzd::nullresult;
}

}
