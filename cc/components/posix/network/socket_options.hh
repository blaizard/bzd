#pragma once

#include "cc/components/posix/error.hh"
#include "cc/bzd/container/result.hh"
#include "cc/components/posix/io/file_descriptor.hh"

namespace bzd::platform::posix::network {

using SocketOption = int;
using SocketOptionLevel = int;
using SocketOptionName = int;

Result<SocketOption, Error> getSocketOptions(const FileDescriptor fd, const SocketOptionLevel level, const SocketOptionName name) noexcept;

Result<void, Error> setSocketOptions(const FileDescriptor fd, const SocketOptionLevel level, const SocketOptionName name, const SocketOption value) noexcept;

/*
int getsockopt(int sockfd, int level, int optname,
                      void *restrict optval, socklen_t *restrict optlen);
int setsockopt(int sockfd, int level, int optname,
                      const void *optval, socklen_t optlen);
*/
}
