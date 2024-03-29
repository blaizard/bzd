#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/components/posix/io/file_descriptor.hh"
#include "cc/components/posix/network/address/address.hh"
#include "cc/components/posix/network/address/family.hh"

#include <sys/socket.h>

namespace bzd::components::posix::network {

class Socket
{
private:
	constexpr explicit Socket(const FileDescriptorOwner::NativeType fd) noexcept : fd_{fd} {}

public:
	Socket() = default;

	/// Create a socket.
	static bzd::Result<Socket, bzd::Error> make(const AddressFamily family, const SocketType type, const int protocol = 0) noexcept;

	/// Create a socket from an address.
	static bzd::Result<Socket, bzd::Error> make(const Address& address,
												const SocketTypeOption options = SocketTypeOption::closeOnExec) noexcept;

public:
	/// Get its associated file descriptor object.
	constexpr FileDescriptor getFileDescriptor() const noexcept { return fd_; }

	/// Bind this socket to an address.
	///
	/// \param address The address to bind this socket to.
	bzd::Result<void, bzd::Error> bind(const Address& address) noexcept;

	/// Listen for connections on this socket.
	///
	/// \param maxPendingConnection The maximum length to which the internal queue of pending connections may grow.
	bzd::Result<void, bzd::Error> listen(const bzd::Size maxPendingConnection) noexcept;

	/// Accept incoming connection.
	bzd::Async<Socket> accept() noexcept;

	void reset() noexcept;

private:
	/// File descriptor associated with this socket.
	bzd::components::posix::FileDescriptorOwner fd_{};
};

} // namespace bzd::components::posix::network
