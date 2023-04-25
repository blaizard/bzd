#pragma once

#include "cc/components/posix/proactor/interface.hh"

namespace bzd::components::posix::mock {

class Proactor : public bzd::components::posix::Proactor<Proactor>
{
public:
	bzd::Async<> write(const FileDescriptor, const bzd::Span<const bzd::Byte>) noexcept { co_return {}; }

	bzd::Async<bzd::Span<const bzd::Byte>> read(const FileDescriptor, bzd::Span<bzd::Byte>&& data) noexcept { co_return data.first(1u); }

	bzd::Async<> connect(const FileDescriptor, const network::Address&) noexcept { co_return {}; }
};

} // namespace bzd::components::posix::mock
