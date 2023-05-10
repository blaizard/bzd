#pragma once

#include "cc/bzd/algorithm/byte_copy.hh"
#include "cc/bzd/container/range/views/take.hh"
#include "cc/bzd/container/string.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/type_traits/is_convertible.hh"
#include "cc/bzd/utility/pattern/formatter/to_string/base.hh"

namespace bzd {

template <class T>
requires(concepts::convertible<T, bzd::StringView> || concepts::convertible<T, bzd::interface::String>)
struct Formatter<T>
{
	struct Metadata
	{
		bool isPrecision = false;
		bzd::Size precision = 6;
	};

	template <bzd::concepts::outputStreamRange Range>
	static constexpr bzd::Optional<bzd::Size> toString(Range&& range,
													   const bzd::StringView value,
													   const Metadata metadata = Metadata{}) noexcept
	{
		return toStringBase(bzd::forward<Range>(range), value, metadata);
	}

	template <bzd::concepts::outputStreamRange Range>
	static constexpr bzd::Optional<bzd::Size> toString(Range&& range,
													   const bzd::interface::String& value,
													   const Metadata metadata = Metadata{}) noexcept
	{
		return toStringBase(bzd::forward<Range>(range), value, metadata);
	}

	template <class Adapter>
	static constexpr Metadata parse(bzd::StringView options) noexcept
	{
		Metadata metadata{};

		while (!options.empty())
		{
			const auto c = options.front();
			options.removePrefix(1);
			switch (c)
			{
			// Parse precision [.precision]
			case '.':
				Adapter::assertTrue(!metadata.isPrecision, "Precision option can only be assigned once.");
				metadata.isPrecision = parseUnsignedInteger(options, metadata.precision);
				break;
			}
		}

		return metadata;
	}

private:
	template <bzd::concepts::outputStreamRange Range, class U>
	static constexpr bzd::Optional<bzd::Size> toStringBase(Range&& range, const U& value, const Metadata metadata) noexcept
	{
		if (metadata.isPrecision)
		{
			const auto size = bzd::min(metadata.precision, value.size());
			if (bzd::algorithm::byteCopyReturnSize(value | bzd::range::take(size), range) == size)
			{
				return size;
			}
		}
		else
		{
			if (bzd::algorithm::byteCopyReturnSize(value, range) == value.size())
			{
				return value.size();
			}
		}
		return bzd::nullopt;
	}

private:
	/// Parse an unsigned integer
	static constexpr bool parseUnsignedInteger(bzd::StringView& format, bzd::Size& integer) noexcept
	{
		bool isDefined = false;
		integer = 0;
		for (; format.size() > 0 && format.front() >= '0' && format.front() <= '9';)
		{
			isDefined = true;
			integer = integer * 10 + (format.front() - '0');
			format.removePrefix(1);
		}
		return isDefined;
	}
};

} // namespace bzd
