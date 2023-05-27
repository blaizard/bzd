#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/utility/pattern/pattern.hh"

namespace bzd::pattern::impl {

template <class Range, class Adapter>
class ProcessorAsync
{
public:
	template <class... Args>
	static constexpr auto make(Args&... args) noexcept
	{
		// Make the actual lambda
		const auto lambdas = bzd::makeTuple([&args](Range& range, const Metadatas<Adapter, Args...>& metadatas) -> bzd::Async<Size> {
			if constexpr (concepts::metadata<Adapter, Args>)
			{
				const auto& metadata = metadatas.template get<Metadata<Adapter, Args>>();
				co_return co_await Adapter::template Specialization<Args>::process(range, args, metadata);
			}
			else
			{
				co_return co_await Adapter::template Specialization<Args>::process(range, args);
			}
		}...);

		return ProcessorType<decltype(lambdas)>{lambdas};
	}

private:
	template <class Lambdas>
	class ProcessorType
	{
	public:
		constexpr ProcessorType(Lambdas& lambdas) noexcept : lambdas_{lambdas} {}

		template <class Fragment>
		bzd::Async<Size> process(Range& range, const Fragment& fragment) const noexcept
		{
			const auto index = fragment.index;
			if constexpr (Lambdas::size() > 0)
			{
				if (index == 0)
				{
					co_return co_await lambdas_.template get<0>()(range, fragment.metadatas);
				}
			}
			if constexpr (Lambdas::size() > 1)
			{
				if (index == 1)
				{
					co_return co_await lambdas_.template get<1>()(range, fragment.metadatas);
				}
			}
			if constexpr (Lambdas::size() > 2)
			{
				if (index == 2)
				{
					co_return co_await lambdas_.template get<2>()(range, fragment.metadatas);
				}
			}
			if constexpr (Lambdas::size() > 3)
			{
				if (index == 3)
				{
					co_return co_await lambdas_.template get<3>()(range, fragment.metadatas);
				}
			}
			if constexpr (Lambdas::size() > 4)
			{
				if (index == 4)
				{
					co_return co_await lambdas_.template get<4>()(range, fragment.metadatas);
				}
			}
			if constexpr (Lambdas::size() > 5)
			{
				if (index == 5)
				{
					co_return co_await lambdas_.template get<5>()(range, fragment.metadatas);
				}
			}
			if constexpr (Lambdas::size() > 6)
			{
				if (index == 6)
				{
					co_return co_await lambdas_.template get<6>()(range, fragment.metadatas);
				}
			}
			if constexpr (Lambdas::size() > 7)
			{
				if (index == 7)
				{
					co_return co_await lambdas_.template get<7>()(range, fragment.metadatas);
				}
			}
			if constexpr (Lambdas::size() > 8)
			{
				if (index == 8)
				{
					co_return co_await lambdas_.template get<8>()(range, fragment.metadatas);
				}
			}
			if constexpr (Lambdas::size() > 9)
			{
				if (index == 9)
				{
					co_return co_await lambdas_.template get<9>()(range, fragment.metadatas);
				}
			}
			static_assert(Lambdas::size() <= 10, "Too many arguments passed to format, not supported.");

			co_return 0u;
		}

	private:
		const Lambdas lambdas_;
	};
};

template <class Range, class Schema, bzd::concepts::constexprStringView Pattern, class... Args>
constexpr auto makeAsync(const Pattern&, Args&... args) noexcept
{
	constexpr auto iterable = parse<Adapter<ConstexprAssert, Schema>, Pattern, Args...>();
	static_assert(iterable.size() > 0, "Compile-time string format check failed.");
	auto processor = ProcessorAsync<Range, Adapter<RuntimeAssert, Schema>>::make(args...);

	return bzd::makeTuple(bzd::move(iterable), bzd::move(processor));
}

} // namespace bzd::pattern::impl
