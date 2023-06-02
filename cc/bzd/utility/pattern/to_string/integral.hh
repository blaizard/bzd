#pragma once

#include "cc/bzd/algorithm/byte_copy.hh"
#include "cc/bzd/container/array.hh"
#include "cc/bzd/container/range/views/reverse.hh"
#include "cc/bzd/container/string.hh"
#include "cc/bzd/type_traits/is_integral.hh"
#include "cc/bzd/type_traits/is_signed.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/pattern/to_string/base.hh"

namespace bzd {

template <concepts::integral T>
struct ToString<T>
{
public:
	struct Metadata
	{
		enum class Sign
		{
			automatic,
			always,
			only_negative
		};

		enum class Format
		{
			automatic,
			binary,
			decimal,
			octal,
			hexadecimalLower,
			hexadecimalUpper
		};

		Sign sign = Sign::automatic;
		bool alternate = false;
		Format format = Format::automatic;
	};

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
			// Parse sign: [sign]
			case '+':
				Adapter::assertTrue(metadata.sign == Metadata::Sign::automatic, "Sign option can only be assigned once.");
				metadata.sign = Metadata::Sign::always;
				break;
			case '-':
				Adapter::assertTrue(metadata.sign == Metadata::Sign::automatic, "Sign option can only be assigned once.");
				metadata.sign = Metadata::Sign::only_negative;
				break;
			// Parse alternate form [#]
			case '#':
				Adapter::assertTrue(!metadata.alternate, "Alternate option can only be assigned once.");
				metadata.alternate = true;
				break;
			// Parse type [type]
			default:
				Adapter::assertTrue(metadata.format == Metadata::Format::automatic, "Conversion format option can only be assigned once.");
				switch (c)
				{
				case 'b':
					metadata.format = Metadata::Format::binary;
					break;
				case 'd':
					metadata.format = Metadata::Format::decimal;
					break;
				case 'o':
					metadata.format = Metadata::Format::octal;
					break;
				case 'x':
					metadata.format = Metadata::Format::hexadecimalLower;
					break;
				case 'X':
					metadata.format = Metadata::Format::hexadecimalUpper;
					break;
				default:
					Adapter::onError("Unsupported option, only the following is supported: [+-#bdoxX]");
				}
			}
		}

		return metadata;
	}

public:
	template <concepts::outputByteCopyableRange Range>
	static constexpr Optional<Size> process(Range&& range, const T& data, const Metadata metadata = Metadata{}) noexcept
	{
		switch (metadata.format)
		{
		case Metadata::Format::binary:
			return toStringBase<2u>(bzd::forward<Range>(range), data, metadata);
		case Metadata::Format::octal:
			return toStringBase<8u>(bzd::forward<Range>(range), data, metadata);
		case Metadata::Format::decimal:
		case Metadata::Format::automatic:
			return toStringBase<10u>(bzd::forward<Range>(range), data, metadata);
		case Metadata::Format::hexadecimalLower:
		case Metadata::Format::hexadecimalUpper:
			return toStringBase<16u>(bzd::forward<Range>(range), data, metadata);
		}
		return bzd::nullopt;
	}

	template <Size base, concepts::outputByteCopyableRange Range>
	static constexpr bzd::Optional<bzd::Size> toStringBase(Range&& range, const T& n, const Metadata& metadata) noexcept
	{
		const auto& digits = (metadata.format == Metadata::Format::hexadecimalUpper) ? digitsUpperCase : digitsLowerCase;

		static_assert(base > 1 && base <= 16, "Invalid base size.");
		static_assert(sizeof(T) <= 8, "Only up to 64-bit integers are supported.");

		bzd::range::Stream stream{bzd::inPlace, range};
		Size count = 0u;
		if (metadata.alternate)
		{
			switch (metadata.format)
			{
			case Metadata::Format::binary:
				if (bzd::algorithm::byteCopyReturnSize("0b"_sv, stream) != 2u)
				{
					return bzd::nullopt;
				}
				count += 2u;
				break;
			case Metadata::Format::octal:
				if (bzd::algorithm::byteCopyReturnSize("0o"_sv, stream) != 2u)
				{
					return bzd::nullopt;
				}
				count += 2u;
				break;
			case Metadata::Format::decimal:
			case Metadata::Format::automatic:
				break;
			case Metadata::Format::hexadecimalLower:
			case Metadata::Format::hexadecimalUpper:
				if (bzd::algorithm::byteCopyReturnSize("0x"_sv, stream) != 2u)
				{
					return bzd::nullopt;
				}
				count += 2u;
				break;
			}
		}

		bzd::String<64u> buffer{};

		T number = n;
		if constexpr (bzd::typeTraits::isSigned<T>)
		{
			number = (n < 0) ? -n : n;
		}

		do
		{
			const auto digit = digits[static_cast<Size>(number % base)];
			number /= base;
			if (!buffer.append(digit))
			{
				return bzd::nullopt;
			}
		} while (number);

		if constexpr (bzd::typeTraits::isSigned<T>)
		{
			if (n < 0)
			{
				if (!buffer.append('-'))
				{
					return bzd::nullopt;
				}
			}
		}

		if (algorithm::byteCopyReturnSize(buffer | bzd::range::reverse(), stream) != buffer.size())
		{
			return bzd::nullopt;
		}
		return buffer.size() + count;
	}

private:
	static constexpr bzd::Array<const char, 16> digitsLowerCase{
		inPlace, '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
	static constexpr bzd::Array<const char, 16> digitsUpperCase{
		inPlace, '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
};

} // namespace bzd
