#pragma once

#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/is_convertible.hh"
#include "cc/bzd/utility/numeric_limits.hh"

namespace bzd {
template <class IntType = bzd::Int32Type, IntType min = 0, IntType max = NumericLimits<IntType>::max()>
class UniformIntDistribution
{
public: // Traits.
	using ResultType = IntType;

public: // Constructors.
	template <class Generator>
	[[nodiscard]] constexpr ResultType operator()(Generator& generator) const noexcept
	{
		return distribution(generator);
	}

private:
	template <class Generator>
	[[nodiscard]] constexpr ResultType distribution(Generator& generator) const noexcept
	{
		static_assert(typeTraits::isConvertible<typename Generator::ResultType, ResultType>,
					  "Types from the generator and the distribution are incompatible.");
		static_assert(Generator::min() == 0, "If non-zero we have handle the offset.");
		constexpr ResultType range = max - min;
		static_assert(range > 0, "The range must be > 0.");
		static_assert(Generator::max() >= range, "The range is too large for the given generator.");
		constexpr ResultType rejectLim = Generator::max() % range;

		ResultType n;
		do
		{
			n = generator();
		} while (n <= rejectLim);
		return (n % range) + min;
	}
};

template <Int32Type min = 0, Int32Type max = NumericLimits<Int32Type>::max()>
UniformIntDistribution() -> UniformIntDistribution<Int32Type, min, max>;

} // namespace bzd
