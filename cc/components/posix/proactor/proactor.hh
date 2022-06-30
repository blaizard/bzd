#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/components/posix/io/file_descriptor.hh"

namespace bzd::platform::posix {

template <class Impl>
class Proactor
{
public:
	/// Write to a file descriptor: https://man7.org/linux/man-pages/man2/write.2.html
	///
	/// \param fd The file descriptor to write to.
	/// \param data The data blob to be written.
	/// \return The number of bytes written.
	bzd::Async<bzd::SizeType> write(const FileDescriptor fd, const bzd::Span<const bzd::ByteType> data) noexcept
	{
		return bzd::impl::getMethod(this, &Proactor::write, &Impl::write)->write(fd, data);
	}
};

} // namespace bzd::platform::posix
