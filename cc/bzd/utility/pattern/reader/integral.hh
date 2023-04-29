#pragma once

#include "cc/bzd/container/array.hh"
#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/range/stream.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/type_traits/is_integral.hh"
#include "cc/bzd/type_traits/is_signed.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/type_traits/remove_reference.hh"
#include "cc/bzd/utility/begin.hh"
#include "cc/bzd/utility/end.hh"
#include "cc/bzd/utility/pattern/reader/base.hh"

namespace bzd {

template <concepts::integral T>
struct Formatter<T>
{
private:
	static constexpr Array<const char, 16> digits_{inPlace, '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

public:
	template <concepts::inputStreamRange Range, Size base = 10, class Digits = decltype(digits_)>
	static constexpr Optional<Size> fromString(Range&& range, T& data, const Digits& digits = digits_) noexcept
	{
		static_assert(base > 1 && base <= 16, "Invalid base size.");
		static_assert(Digits::size() >= base, "There is not enough digits for the base.");

		data = 0;
		Size index = 0u;
		auto it = bzd::begin(range);
		const auto last = bzd::end(range);

		Bool isNegative = false;
		if (it != last && *it == '-')
		{
			if (!concepts::isSigned<typeTraits::RemoveReference<T>>)
			{
				return bzd::nullopt;
			}
			isNegative = true;
			++it;
		}

		while (!(it == last))
		{
			const Size value = *it - digits[0];
			if (value >= base)
			{
				break;
			}
			data = data * base + value;
			++it;
			++index;
		}

		if (index)
		{
			if (isNegative)
			{
				data = -data;
				return index + 1u;
			}
			return index;
		}
		return bzd::nullopt;
	}
};

} // namespace bzd
