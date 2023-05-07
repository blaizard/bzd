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
#include "cc/bzd/utility/pattern/matcher/base.hh"

namespace bzd {

template <concepts::integral T>
struct Matcher<T>
{
public:
	struct Metadata
	{
		enum class Format
		{
			binary,
			decimal,
			octal,
			hexadecimalLower,
			hexadecimalUpper,
		};
		Format format{Format::decimal};
	};

	template <concepts::inputStreamRange Range>
	static constexpr Optional<Size> fromString(Range&& range, T& data, const Metadata metadata = Metadata{}) noexcept
	{
		switch (metadata.format)
		{
		case Metadata::Format::binary:
			return fromStringBase<Range, 2u>(bzd::forward<Range>(range), data, metadata);
		case Metadata::Format::octal:
			return fromStringBase<Range, 8u>(bzd::forward<Range>(range), data, metadata);
		case Metadata::Format::decimal:
			return fromStringBase<Range, 10u>(bzd::forward<Range>(range), data, metadata);
		case Metadata::Format::hexadecimalLower:
		case Metadata::Format::hexadecimalUpper:
			return fromStringBase<Range, 16u>(bzd::forward<Range>(range), data, metadata);
		}
		return bzd::nullopt;
	}

	template <class Adapter>
	static constexpr Metadata parse(bzd::StringView options) noexcept
	{
		Metadata metadata{};

		while (!options.empty())
		{
			switch (options.front())
			{
			// Format
			case 'b':
				metadata.format = Metadata::Format::binary;
				break;
			case 'o':
				metadata.format = Metadata::Format::octal;
				break;
			case 'd':
				metadata.format = Metadata::Format::decimal;
				break;
			case 'x':
				metadata.format = Metadata::Format::hexadecimalLower;
				break;
			case 'X':
				metadata.format = Metadata::Format::hexadecimalUpper;
				break;
			default:
				Adapter::onError("Unsupported option, only the following is supported: [bodxX]");
			}
			options.removePrefix(1);
		}

		return metadata;
	}

private:
	template <concepts::inputStreamRange Range, Size base>
	static constexpr Optional<Size> fromStringBase(Range&& range, T& data, const Metadata metadata) noexcept
	{
		static_assert(base > 1 && base <= 16, "Invalid base size.");

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
			Size value = *it - '0';
			if constexpr (base > 10)
			{
				if (value >= 10)
				{
					switch (metadata.format)
					{
					case Metadata::Format::hexadecimalLower:
						value -= 'a' - '0' - 10u;
						break;
					case Metadata::Format::hexadecimalUpper:
						value -= 'A' - '0' - 10u;
						break;
					case Metadata::Format::binary:
					case Metadata::Format::octal:
					case Metadata::Format::decimal:
						value = base;
						break;
					}
				}
			}
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
