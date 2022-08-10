#pragma once

#include "cc/components/posix/proactor/interface.hh"

namespace bzd::platform::posix::mock {

class Proactor : public bzd::platform::posix::Proactor<Proactor>
{
public:
	bzd::Async<> write(const FileDescriptor, const bzd::Span<const bzd::Byte>) noexcept { co_return {}; }

	bzd::Async<bzd::Span<bzd::Byte>> read(const FileDescriptor, const bzd::Span<bzd::Byte> data) noexcept { co_return data.first(1U); }

	bzd::Async<> connect(const FileDescriptor, const network::Address&) noexcept { co_return {}; }
};

} // namespace bzd::platform::posix::mock
