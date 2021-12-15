#pragma once

#include "cc/bzd/platform/types.hh"

namespace bzd {
// Implementation of https://en.wikipedia.org/wiki/Linear_congruential_generator
// The default values are the one used by glibc.
template <class UIntType = bzd::UInt32Type, UIntType a = 1103515245, UIntType c = 12345, UIntType m = 2147483648>
class LinearCongruentialEngine
{
public: // Traits.
	using ResultType = UIntType;

public: // Constructor.
	constexpr explicit LinearCongruentialEngine(const UIntType seed = 0) noexcept : value_{seed} {}

public:
	static constexpr ResultType min() noexcept { return 0; }

	static constexpr ResultType max() noexcept { return m - 1; }

	[[nodiscard]] constexpr ResultType operator()() noexcept
	{
		value_ = (a * value_ + c) % m;
		return value_;
	}

private:
	ResultType value_{};
};
} // namespace bzd
