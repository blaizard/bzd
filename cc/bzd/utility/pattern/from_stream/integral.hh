#pragma once

#include "cc/bzd/type_traits/is_integral.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/pattern/from_stream/base.hh"
#include "cc/bzd/utility/pattern/from_string/integral.hh"

namespace bzd {

template <concepts::generatorInputByteCopyableRange Generator, class Callback>
bzd::Async<Size> forEach(Generator&& generator, Callback&& callback) noexcept
{
	auto result = co_await generator;
	if (!result)
	{
		co_return bzd::move(result).propagate();
	}
	auto it = bzd::begin(result.valueMutable());
	auto end = bzd::end(result.valueMutable());
	Size count{0u};
	while (!generator.isCompleted())
	{
		if (it == end)
		{
			result = co_await generator;
			if (result)
			{
				it = bzd::begin(result.valueMutable());
				end = bzd::end(result.valueMutable());
				// If there are still no input after fetching new data.
				if (it == end)
				{
					break;
				}
			}
			else if (result.error().getType() == bzd::ErrorType::eof)
			{
				break;
			}
			else
			{
				co_return bzd::move(result).propagate();
			}
		}

		if (!callback(*it))
		{
			break;
		}

		++count;
		++it;
	}

	co_return count;
}

template <concepts::integral T>
struct FromStream<T> : public FromString<T>
{
	using Metadata = typename bzd::FromString<T>::Metadata;

	template <concepts::generatorInputByteCopyableRange Generator>
	static bzd::Async<Size> process(Generator&& generator, T& output, const Metadata metadata = Metadata{}) noexcept
	{
		output = 0u;
		bzd::Size count{};
		bzd::Bool isNegative{false};

		if constexpr (concepts::isSigned<typeTraits::RemoveReference<T>>)
		{
			co_await !forEach(generator, [&](const auto input) -> Bool {
				const char c = static_cast<char>(input);
				if (c == '-')
				{
					isNegative = true;
					return true;
				}
				return false;
			});
		}

		switch (metadata.format)
		{
		case Metadata::Format::binary:
			count = co_await !forEach(bzd::forward<Generator>(generator), [&](const auto input) -> Bool {
				const char c = static_cast<char>(input);
				if (c >= '0' && c <= '1')
				{
					output = output * 2 + (c - '0');
					return true;
				}
				return false;
			});
			break;
		case Metadata::Format::octal:
			count = co_await !forEach(bzd::forward<Generator>(generator), [&](const auto input) -> Bool {
				const char c = static_cast<char>(input);
				if (c >= '0' && c <= '7')
				{
					output = output * 8 + (c - '0');
					return true;
				}
				return false;
			});
			break;
		case Metadata::Format::decimal:
			count = co_await !forEach(bzd::forward<Generator>(generator), [&](const auto input) -> Bool {
				const char c = static_cast<char>(input);
				if (c >= '0' && c <= '9')
				{
					output = output * 10 + (c - '0');
					return true;
				}
				return false;
			});
			break;
		case Metadata::Format::hexadecimalLower:
			count = co_await !forEach(bzd::forward<Generator>(generator), [&](const auto input) -> Bool {
				const char c = static_cast<char>(input);
				if (c >= '0' && c <= '9')
				{
					output = output * 16 + (c - '0');
					return true;
				}
				if (c >= 'a' && c <= 'f')
				{
					output = output * 16 + (c - 'a' + 10);
					return true;
				}
				return false;
			});
			break;
		case Metadata::Format::hexadecimalUpper:
			count = co_await !forEach(bzd::forward<Generator>(generator), [&](const auto input) -> Bool {
				const char c = static_cast<char>(input);
				if (c >= '0' && c <= '9')
				{
					output = output * 16 + (c - '0');
					return true;
				}
				if (c >= 'A' && c <= 'F')
				{
					output = output * 16 + (c - 'A' + 10);
					return true;
				}
				return false;
			});
			break;
		};
		if (!count)
		{
			co_return bzd::error::Failure{"No integral"};
		}
		if (isNegative)
		{
			output = -output;
			++count;
		}
		co_return count;
	}
};

} // namespace bzd
