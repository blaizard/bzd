#pragma once

#include "cc/components/posix/error.hh"
#include "cc/components/posix/proactor/interface.hh"

#include <cerrno>
#include <cstring>
#include <sys/epoll.h>
#include <unistd.h>

namespace bzd::platform::posix::sync {

/// Proactor for basic POSIX functionalities
class Proactor : public bzd::platform::posix::Proactor<Proactor>
{
public:
	/// Perform an asynchronous write operator.
	// NOLINTNEXTLINE(bugprone-exception-escape)
	bzd::Async<bzd::Size> write(const FileDescriptor fd, const bzd::Span<const bzd::Byte> data) noexcept
	{
		bzd::Size size{0u};
		while (size < data.size())
		{
			const auto result = ::write(fd.native(), &data.at(size), data.size() - size);
			if (result < 0)
			{
				co_return bzd::error::Posix("write");
			}
			size += result;
		}
		co_return size;
	}
};

} // namespace bzd::platform::posix::sync
