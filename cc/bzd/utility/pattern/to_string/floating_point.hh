#pragma once

#include "cc/bzd/algorithm/byte_copy.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/type_traits/is_floating_point.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/pattern/to_string/base.hh"
#include "cc/bzd/utility/pattern/to_string/integral.hh"

namespace bzd {

template <concepts::floatingPoint T>
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
			fixedPoint,
			fixedPointPercent,
		};

		Sign sign = Sign::automatic;
		bool isPrecision = false;
		bzd::Size precision = 6;
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
			// Parse precision [.precision]
			case '.':
				Adapter::assertTrue(!metadata.isPrecision, "Precision option can only be assigned once.");
				metadata.isPrecision = parseUnsignedInteger(options, metadata.precision);
				break;
			// Parse type [type]
			default:
				Adapter::assertTrue(metadata.format == Metadata::Format::automatic, "Conversion format option can only be assigned once.");
				switch (c)
				{
				case 'f':
					metadata.format = Metadata::Format::fixedPoint;
					break;
				case '%':
					metadata.format = Metadata::Format::fixedPointPercent;
					break;
				default:
					Adapter::onError("Unsupported option, only the following is supported: [+-.<precision>f%]");
				}
			}
		}

		return metadata;
	}

public:
	template <concepts::outputByteCopyableRange Range>
	static constexpr bzd::Optional<bzd::Size> process(Range&& range, const T& n, const Size precision) noexcept
	{
		Metadata metadata{};
		metadata.precision = precision;
		return process(bzd::forward<Range>(range), n, metadata);
	}

	template <concepts::outputByteCopyableRange Range>
	static constexpr bzd::Optional<bzd::Size> process(Range&& range, const T& n, const Metadata metadata = Metadata{}) noexcept
	{
		bzd::ranges::Stream stream{bzd::inPlace, range};
		switch (metadata.format)
		{
		case Metadata::Format::automatic:
		case Metadata::Format::fixedPoint:
			return toStringBase(stream, n, metadata);
		case Metadata::Format::fixedPointPercent:
		{
			const auto result = toStringBase(stream, n * 100., metadata);
			if (result)
			{
				if (bzd::algorithm::byteCopyReturnSize("%"_sv, stream))
				{
					return result.value() + 1u;
				}
			}
		}
		}
		return bzd::nullopt;
	}

private:
	template <concepts::outputByteCopyableRange Range>
	static constexpr bzd::Optional<bzd::Size> toStringBase(Range&& range, const T& n, const Metadata& metadata) noexcept
	{
		constexpr const T resolutionList[15] = {1,
												0.1,
												0.01,
												0.001,
												0.0001,
												0.00001,
												0.000001,
												0.0000001,
												0.00000001,
												0.000000001,
												0.0000000001,
												0.00000000001,
												0.000000000001,
												0.0000000000001,
												0.00000000000001};

		auto resolution = resolutionList[metadata.precision];
		const T roundedNumber = n + resolution / 2;

		const auto result = bzd::toString(range, static_cast<bzd::Int32>(roundedNumber));
		if (!result)
		{
			return bzd::nullopt;
		}

		if (!algorithm::byteCopyReturnSize("."_sv, range))
		{
			return bzd::nullopt;
		}

		auto counter = result.value() + 1u;
		auto afterComma = (roundedNumber - static_cast<T>(static_cast<int>(n)));
		while (afterComma >= resolution * 2)
		{
			afterComma *= 10;
			resolution *= 10;

			const auto digit = static_cast<int>(afterComma);
			const char array[1] = {static_cast<char>('0' + digit)};
			if (!algorithm::byteCopyReturnSize(array, range))
			{
				return bzd::nullopt;
			}
			++counter;
			afterComma -= static_cast<T>(digit);
		}

		return counter;
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
