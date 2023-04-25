#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/components/posix/io/file_descriptor.hh"
#include "cc/components/posix/network/address/address.hh"

namespace bzd::components::posix {

template <class Impl>
class Proactor
{
public:
	/// Write to a file descriptor: https://man7.org/linux/man-pages/man2/write.2.html
	///
	/// \param fd The file descriptor to write to.
	/// \param data The data blob to be written.
	bzd::Async<> write(const FileDescriptor fd, const bzd::Span<const bzd::Byte> data) noexcept
	{
		return bzd::impl::getImplementation(this, &Proactor::write, &Impl::write)->write(fd, data);
	}

	/// Perform an asynchronous read operator: https://man7.org/linux/man-pages/man2/read.2.html
	///
	/// This function blocks until there is at least a single byte of data to be read.
	/// It will return with at least 1 byte or more depending on the implementation.
	///
	/// \param fd The file descriptor to read from.
	/// \param data The data blob to receive data.
	/// \return The span of data read.
	bzd::Async<bzd::Span<const bzd::Byte>> read(const FileDescriptor fd, bzd::Span<bzd::Byte>&& data) noexcept
	{
		return bzd::impl::getImplementation(this, &Proactor::read, &Impl::read)->read(fd, bzd::move(data));
	}

	/// Perform an asynchronous connect operator: https://man7.org/linux/man-pages/man2/connect.2.html
	///
	/// \param fd The file descriptor to connect.
	/// \param address The address to connect this file descriptor to.
	bzd::Async<> connect(const FileDescriptor fd, const network::Address& address) noexcept
	{
		return bzd::impl::getImplementation(this, &Proactor::connect, &Impl::connect)->connect(fd, address);
	}
};

} // namespace bzd::components::posix
