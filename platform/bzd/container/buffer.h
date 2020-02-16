#pragma once

#include "bzd/container/span.h"
#include "bzd/container/string_view.h"

namespace bzd {
class Buffer : public bzd::Span<bzd::UInt8Type>
{
public:
	using bzd::Span<bzd::UInt8Type>::Span;

	//constexpr Buffer(const bzd::StringView& str) : bzd::Span<bzd::UInt8Type>(reinterpret_cast<bzd::UInt8Type*>(str.data()), str.size()) {}
};
} // namespace bzd
