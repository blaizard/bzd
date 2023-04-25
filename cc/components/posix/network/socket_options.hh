#pragma once

#include "cc/bzd/container/result.hh"
#include "cc/components/posix/error.hh"
#include "cc/components/posix/io/file_descriptor.hh"

namespace bzd::components::posix::network {

using SocketOption = int;
using SocketOptionLevel = int;
using SocketOptionName = int;

Result<SocketOption, Error> getSocketOptions(const FileDescriptor fd, const SocketOptionLevel level, const SocketOptionName name) noexcept;

Result<void, Error> setSocketOptions(const FileDescriptor fd,
									 const SocketOptionLevel level,
									 const SocketOptionName name,
									 const SocketOption value) noexcept;

} // namespace bzd::components::posix::network
