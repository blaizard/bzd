#pragma once

#include "cc/components/std/stream/out/interface.hh"

#include <iostream>
#include <unistd.h>

namespace bzd::platform::std {
class Out : public bzd::OStream
{
public:
	template <class Context>
	constexpr explicit Out(Context&) noexcept
	{
	}

	bzd::Async<> write(const bzd::Span<const bzd::Byte> data) noexcept final
	{
		::std::cout.write(reinterpret_cast<const char*>(data.data()), static_cast<::std::streamsize>(data.size()));
		::std::flush(::std::cout);
		co_return {};
	}
};
} // namespace bzd::platform::std
