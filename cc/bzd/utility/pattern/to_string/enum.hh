#pragma once

#include "cc/bzd/utility/pattern/to_string/integral.hh"
#include "cc/bzd/type_traits/is_enum.hh"
#include "cc/bzd/type_traits/underlying_type.hh"

namespace bzd {

template <bzd::concepts::isEnum T>
struct ToString<T>
{
	template <bzd::concepts::outputByteCopyableRange Range>
	static constexpr bzd::Optional<bzd::Size> process(Range&& range, const T value) noexcept
	{
		using UnderlyingType = typeTraits::UnderlyingType<T>;
		return ToString<UnderlyingType>::process(bzd::forward<Range>(range), static_cast<UnderlyingType>(value));
	}
};

} // namespace bzd
