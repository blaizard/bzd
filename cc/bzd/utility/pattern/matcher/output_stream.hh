#pragma once

#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/pattern/matcher/base.hh"
#include "cc/bzd/utility/regexpr/regexpr.hh"

namespace bzd {

template <concepts::outputStreamRange Output>
struct Matcher<Output>
{
	struct Metadata
	{
		bzd::StringView regexpr{"[^\\w]"};
	};

	template <bzd::concepts::inputStreamRange Range>
	static constexpr Optional<Size> fromString(Range&& range, Output& output, const Metadata metadata = Metadata{}) noexcept
	{
		bzd::Regexpr regexpr{metadata.regexpr};
		if (const auto result = regexpr.capture(range, output); result)
		{
			return result.value();
		}
		return bzd::nullopt;
	}

	template <class Adapter>
	static constexpr Metadata parse(const bzd::StringView options) noexcept
	{
		Metadata metadata{};
		if (!options.empty())
		{
			metadata.regexpr = options;
		}
		return metadata;
	}
};

} // namespace bzd
