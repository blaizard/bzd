#pragma once

#include "cc/bzd/platform/types.hh"

namespace bzd {

// Implementation of the xorwow, a variant of the xorshift: https://en.wikipedia.org/wiki/Xorshift
class XorwowEngine
{
public: // Traits.
	using ResultType = UInt32Type;

public: // Constructor.
	constexpr explicit XorwowEngine(const ResultType seed = 0) noexcept :
		state_{{0xc22de739, 0x9f309919, 0xa3c1b981, 0x2baf1238, 0xefd85aeb}, seed}
	{
	}

public:
	static constexpr ResultType min() noexcept { return 0; }

	static constexpr ResultType max() noexcept { return 0xffffffff; }

	[[nodiscard]] constexpr ResultType operator()() noexcept
	{
		auto t = state_.x[4];
		const auto s = state_.x[0];
		state_.x[4] = state_.x[3];
		state_.x[3] = state_.x[2];
		state_.x[2] = state_.x[1];
		state_.x[1] = s;
		t ^= t >> 2;
		t ^= t << 1;
		t ^= s ^ (s << 4);
		state_.x[0] = t;
		state_.counter += 362437;
		return t + state_.counter;
	}

private:
	struct
	{
		ResultType x[5];
		ResultType counter;
	} state_;
};
} // namespace bzd
