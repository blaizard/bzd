#pragma once

#include "cc/bzd/utility/owner.hh"
#include "cc/bzd/utility/to_underlying.hh"
#include "cc/components/posix/network/address/family.hh"
#include "cc/components/posix/posix.hh"

#include <sys/socket.h>

namespace bzd::platform::posix::network {

enum class SocketType
{
	/// Provides sequenced, reliable, two-way, connection-based
	/// byte streams.  An out-of-band data transmission mechanism
	/// may be supported.
	stream = SOCK_STREAM,
	/// Supports datagrams (connectionless, unreliable messages of
	/// a fixed maximum length).
	datagram = SOCK_DGRAM,
	/// Provides a sequenced, reliable, two-way connection-based
	/// data transmission path for datagrams of fixed maximum
	/// length; a consumer is required to read an entire packet
	/// with each input system call.
	sequencedPacket = SOCK_SEQPACKET,
	/// Provides raw network protocol access.
	raw = SOCK_RAW,
	/// Provides a reliable datagram layer that does not guarantee
	/// ordering.
	reliable = SOCK_RDM,
};

/// The followings are bitwise OR options.
enum class SocketTypeOption
{
	/// Set the O_NONBLOCK file status flag on the open file
	/// description (see open(2)) referred to by the new file
	/// descriptor.  Using this flag saves extra calls to fcntl(2)
	/// to achieve the same result.
	nonBlocking = SOCK_NONBLOCK,
	/// Set the close-on-exec (FD_CLOEXEC) flag on the new file
	/// descriptor.  See the description of the O_CLOEXEC flag in
	/// open(2) for reasons why this may be useful.
	closeOnExec = SOCK_CLOEXEC
};

constexpr SocketType operator|(const SocketType a, const SocketTypeOption b) noexcept
{
	return static_cast<SocketType>(bzd::toUnderlying(a) | bzd::toUnderlying(b));
}

class Socket : public bzd::Owner<Socket>
{
private:
	constexpr explicit Socket(FileDescriptor fd) noexcept : fd_{fd} {}

public:
	constexpr Socket(Socket&& socket) noexcept : fd_{socket.fd_} { socket.fd_ = -1; }
	constexpr Socket& operator=(Socket&&) noexcept = delete;

	/// Create a socket.
	static bzd::Result<Socket, bzd::Error> make(const AddressFamily family, const SocketType type, const int protocol) noexcept;

public:
	/// Get it associated file descriptor object.
	constexpr FileDescriptor getFileDescriptor() const noexcept { return fd_; }

	~Socket() noexcept;

private:
	/// File descriptor associated with this socket.
	FileDescriptor fd_{-1};
};

} // namespace bzd::platform::posix::network
