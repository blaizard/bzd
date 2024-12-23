#pragma once

#include "cc/bzd/type_traits/is_integral.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/pattern/from_stream/base.hh"
#include "cc/bzd/utility/pattern/from_string/integral.hh"

namespace bzd {

template <concepts::integral T>
struct FromStream<T> : public FromString<T>
{
	using Metadata = typename bzd::FromString<T>::Metadata;

	template <concepts::asyncInputByteCopyableRange Generator>
	static bzd::Async<Size> process(Generator&& generator, T& output, const Metadata metadata = Metadata{}) noexcept
	{
		output = 0u;
		bzd::Size count{};
		bzd::Bool isNegative{false};

		if constexpr (concepts::isSigned<typeTraits::RemoveReference<T>>)
		{
			co_await !bzd::async::forEach(generator, [&](const auto input) -> Bool {
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
			co_await !bzd::async::forEach(bzd::forward<Generator>(generator), [&](const auto input) -> Bool {
				const char c = static_cast<char>(input);
				if (c >= '0' && c <= '1')
				{
					++count;
					output = output * 2 + (c - '0');
					return true;
				}
				return false;
			});
			break;
		case Metadata::Format::octal:
			co_await !bzd::async::forEach(bzd::forward<Generator>(generator), [&](const auto input) -> Bool {
				const char c = static_cast<char>(input);
				if (c >= '0' && c <= '7')
				{
					++count;
					output = output * 8 + (c - '0');
					return true;
				}
				return false;
			});
			break;
		case Metadata::Format::decimal:
			co_await !bzd::async::forEach(bzd::forward<Generator>(generator), [&](const auto input) -> Bool {
				const char c = static_cast<char>(input);
				if (c >= '0' && c <= '9')
				{
					++count;
					output = output * 10 + (c - '0');
					return true;
				}
				return false;
			});
			break;
		case Metadata::Format::hexadecimalLower:
			co_await !bzd::async::forEach(bzd::forward<Generator>(generator), [&](const auto input) -> Bool {
				const char c = static_cast<char>(input);
				if (c >= '0' && c <= '9')
				{
					++count;
					output = output * 16 + (c - '0');
					return true;
				}
				if (c >= 'a' && c <= 'f')
				{
					++count;
					output = output * 16 + (c - 'a' + 10);
					return true;
				}
				return false;
			});
			break;
		case Metadata::Format::hexadecimalUpper:
			co_await !bzd::async::forEach(bzd::forward<Generator>(generator), [&](const auto input) -> Bool {
				const char c = static_cast<char>(input);
				if (c >= '0' && c <= '9')
				{
					++count;
					output = output * 16 + (c - '0');
					return true;
				}
				if (c >= 'A' && c <= 'F')
				{
					++count;
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
