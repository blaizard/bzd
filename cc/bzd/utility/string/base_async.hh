#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/utility/string/base.hh"

namespace bzd::format::impl {

template <class Range, class Adapter>
class ProcessorAsync
{
public:
	template <class... Args>
	static constexpr auto make(const Args&... args) noexcept
	{
		// Make the actual lambda
		const auto lambdas = bzd::makeTuple([&args](Range& range, const Metadata& metadata) -> bzd::Async<> {
			co_await !Adapter::process(range, args, metadata);
			co_return {};
		}...);

		return ProcessorType<decltype(lambdas)>{lambdas};
	}

private:
	template <class Lambdas>
	class ProcessorType
	{
	public:
		constexpr ProcessorType(Lambdas& lambdas) noexcept : lambdas_{lambdas} {}

		bzd::Async<> process(Range& range, const Metadata& metadata) const noexcept
		{
			const auto index = metadata.index;
			if constexpr (Lambdas::size() > 0)
			{
				if (index == 0)
				{
					co_await !lambdas_.template get<0>()(range, metadata);
				}
			}
			if constexpr (Lambdas::size() > 1)
			{
				if (index == 1)
				{
					co_await !lambdas_.template get<1>()(range, metadata);
				}
			}
			if constexpr (Lambdas::size() > 2)
			{
				if (index == 2)
				{
					co_await !lambdas_.template get<2>()(range, metadata);
				}
			}
			if constexpr (Lambdas::size() > 3)
			{
				if (index == 3)
				{
					co_await !lambdas_.template get<3>()(range, metadata);
				}
			}
			if constexpr (Lambdas::size() > 4)
			{
				if (index == 4)
				{
					co_await !lambdas_.template get<4>()(range, metadata);
				}
			}
			if constexpr (Lambdas::size() > 5)
			{
				if (index == 5)
				{
					co_await !lambdas_.template get<5>()(range, metadata);
				}
			}
			if constexpr (Lambdas::size() > 6)
			{
				if (index == 6)
				{
					co_await !lambdas_.template get<6>()(range, metadata);
				}
			}
			if constexpr (Lambdas::size() > 7)
			{
				if (index == 7)
				{
					co_await !lambdas_.template get<7>()(range, metadata);
				}
			}
			if constexpr (Lambdas::size() > 8)
			{
				if (index == 8)
				{
					co_await !lambdas_.template get<8>()(range, metadata);
				}
			}
			if constexpr (Lambdas::size() > 9)
			{
				if (index == 9)
				{
					co_await !lambdas_.template get<9>()(range, metadata);
				}
			}
			static_assert(Lambdas::size() <= 10, "Too many arguments passed to format, not supported.");

			co_return {};
		}

	private:
		const Lambdas lambdas_;
	};
};

template <class Range, class Formatter, class Schema, bzd::concepts::constexprStringView Pattern, class... Args>
constexpr auto makeAsync(const Pattern&, Args&... args) noexcept
{
	// Compile-time format check
	constexpr const bzd::meta::Tuple<Args...> tuple{};
	constexpr const bool isValid = bzd::format::impl::contextValidate<Range, Formatter, Schema>(Pattern::value(), tuple);
	// This line enforces compilation time evaluation
	static_assert(isValid, "Compile-time string format check failed.");

	constexpr bzd::format::impl::Parser<bzd::format::impl::Adapter<bzd::format::impl::NoAssert, Formatter, Schema>> parser{
		Pattern::value()};
	const auto processor =
		bzd::format::impl::ProcessorAsync<Range, bzd::format::impl::Adapter<bzd::format::impl::RuntimeAssert, Formatter, Schema>>::make(
			args...);

	return bzd::makeTuple(bzd::move(parser), bzd::move(processor));
}

} // namespace bzd::format::impl
