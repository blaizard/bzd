#pragma once

#include "cc/bzd/algorithm/byte_copy.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/utility/pattern/formatter/to_string/base.hh"
#include "cc/bzd/utility/pattern/pattern.hh"

namespace bzd {

template <concepts::constexprStringView Pattern>
struct Formatter<Pattern>
{
public:
	/// \brief String formating.
	///
	/// Lightweight and compilation time checking string formating utility.
	/// The syntax is compatible with Python format with some limitations.
	///
	/// \code
	/// format_spec ::=  [sign][#][.precision][type]
	/// sign        ::=  "+" | "-" | " "
	/// precision   ::=  integer
	/// type        ::=  "b" | "d" | "f" | "o" | "x" | "X" | "f" | "p" | "%"
	/// d	Decimal integer
	/// b	Binary format
	/// o	Octal format
	/// x	Hexadecimal format (lower case)
	/// X	Hexadecimal format (upper case)
	/// f	Displays fixed point number (Default: 6)
	/// p   Pointer
	/// %	Percentage. Multiples by 100 and puts % at the end.
	/// \endcode
	///
	/// This is an after text
	///
	/// \param out Output stream where the formating string will be written to.
	/// \param str run-time or compile-time string containing the format.
	/// \param args Arguments to be passed for the format.
	template <bzd::concepts::outputStreamRange Range, class... Args>
	static constexpr bzd::Optional<bzd::Size> toString(Range&& range, const Pattern& pattern, const Args&... args) noexcept
	{
		const auto [parser, processor] = bzd::pattern::impl::make<Range&, StringFormatter, Schema>(pattern, args...);
		bzd::Size count{0u};

		// Run-time call
		for (const auto& result : parser)
		{
			if (!result.str.empty())
			{
				if (bzd::algorithm::byteCopyReturnSize(result.str, range) != result.str.size())
				{
					return bzd::nullopt;
				}
				count += result.str.size();
			}
			if (result.isMetadata)
			{
				const auto maybeCount = processor.process(range, result);
				if (!maybeCount)
				{
					return bzd::nullopt;
				}
				count += maybeCount.value();
			}
		}

		return count;
	}

private:
	class StringFormatter
	{
	public:
		template <class... Args>
		static constexpr bzd::Optional<Size> process(Args&&... args) noexcept
		{
			return ::bzd::toString(bzd::forward<Args>(args)...);
		}
	};

	class Schema
	{
	public:
		/// Get the specialization associated with a type.
		template <class Value>
		using Specialization = typename typeTraits::template Formatter<Value>;
	};
};

template <>
struct Formatter<char>
{
public:
	template <bzd::concepts::outputStreamRange Range>
	static constexpr bzd::Optional<bzd::Size> toString(Range&& range, const char c) noexcept
	{
		if (!bzd::algorithm::byteCopyReturnSize(bzd::StringView{&c, 1u}, range))
		{
			return bzd::nullopt;
		}
		return 1u;
	}
};

} // namespace bzd
