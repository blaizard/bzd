#pragma once

#include "cc/bzd/algorithm/find_if.hh"
#include "cc/bzd/container/array.hh"
#include "cc/bzd/container/string.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/type_traits/is_enum.hh"
#include "cc/bzd/type_traits/underlying_type.hh"
#include "cc/bzd/utility/pattern/to_string/pattern.hh"
#include "cc/bzd/utility/ranges/views/reverse.hh"

namespace {

template <typename E, E V>
constexpr auto enumValueToString() noexcept
{
#if defined(__GNUC__) || defined(__clang__)
	constexpr bzd::StringView raw{__PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__) - 2};
#else
#error "Compiler not supported."
#endif
	constexpr auto start =
		bzd::algorithm::findIf(raw | bzd::ranges::reverse(), [](const auto c) { return c == ':' || c == ')' || c == ' '; });
	if constexpr (*start != ':') // Invalid enum value.
	{
		return bzd::String<1u>{};
	}
	else
	{
		constexpr auto view = raw.subStr(&(*start) - raw.data() + 1u);
		return bzd::String<view.size()>{view};
	}
}

} // namespace

namespace bzd {

template <bzd::concepts::isEnum T>
struct ToString<T>
{
public:
	/// Print enum and their corresponding string.
	///
	/// Only supports printing enum strings that are automatically assigned.
	template <bzd::concepts::outputByteCopyableRange Range>
	static constexpr bzd::Optional<bzd::Size> process(Range&& range, const T value) noexcept
	{
		constexpr auto pattern{"{} ({})"_csv};
		const auto [valueTyped, string] = preprocess(value);
		return ToString<decltype(pattern)>::process(bzd::forward<Range>(range), pattern, string, valueTyped);
	}

protected:
	using UnderlyingType = typeTraits::UnderlyingType<T>;

	struct PreprocessResult
	{
		UnderlyingType value;
		bzd::StringView string{"<unknown>"_sv};
	};

	static constexpr PreprocessResult preprocess(const T value) noexcept
	{
		const UnderlyingType valueTyped = static_cast<UnderlyingType>(value);
		for (const auto [enumValue, enumString] : makeEntries(valuesAsType()))
		{
			if (enumValue == valueTyped)
			{
				return {
					valueTyped,
					enumString,
				};
			}
		}
		return {valueTyped};
	}

private:
	template <UnderlyingType value>
	inline static constexpr auto valueAsString = enumValueToString<T, static_cast<T>(value)>();

	template <UnderlyingType value>
	static constexpr bool isValid() noexcept
	{
		return !enumValueToString<T, static_cast<T>(value)>().empty();
	}

	template <UnderlyingType... values>
	struct ValuesAsType
	{
	};

	template <UnderlyingType value, UnderlyingType... values>
	static constexpr auto concatenateValues(ValuesAsType<value>, ValuesAsType<values...>) noexcept
	{
		return ValuesAsType<value, values...>{};
	}

	template <UnderlyingType value = 0>
	static constexpr auto valuesAsType() noexcept
	{
		constexpr bool isValidValue = isValid<value>();
		if constexpr (isValidValue)
		{
			constexpr auto previousValues = valuesAsType<value + 1>();
			return concatenateValues(ValuesAsType<value>{}, previousValues);
		}
		else
		{
			return ValuesAsType<>{};
		}
	}

	struct Entry
	{
		UnderlyingType value;
		bzd::StringView string;
	};

	template <UnderlyingType... values>
	static auto makeEntries(ValuesAsType<values...>) noexcept
	{
		return bzd::Array<Entry, sizeof...(values)>{
			{{values, bzd::StringView{valueAsString<values>.data(), valueAsString<values>.size()}}...}};
	}
};

} // namespace bzd
