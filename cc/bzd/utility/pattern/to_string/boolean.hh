#pragma once

#include "cc/bzd/algorithm/byte_copy.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/pattern/to_string/base.hh"

namespace bzd {

template <>
struct ToString<bzd::Bool>
{
	template <bzd::concepts::outputStreamRange Range>
	static constexpr bzd::Optional<bzd::Size> process(Range&& range, const bzd::Bool value) noexcept
	{
		const auto string = (value) ? "true"_sv.asBytes() : "false"_sv.asBytes();
		if (bzd::algorithm::byteCopyReturnSize(string, range) != string.size())
		{
			return bzd::nullopt;
		}
		return string.size();
	}
};

} // namespace bzd
