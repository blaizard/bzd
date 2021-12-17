#pragma once

namespace bzd::test {

/// Type that implements a deleted default constructor.
class NoDefaultConstructor
{
protected:
	using Self = NoDefaultConstructor;

public:
	constexpr NoDefaultConstructor() noexcept = delete;
	constexpr NoDefaultConstructor(int) noexcept {}
	constexpr NoDefaultConstructor(const Self& copy) noexcept = default;
	constexpr Self& operator=(const Self& copy) noexcept = default;
	constexpr NoDefaultConstructor(Self&&) noexcept = default;
	constexpr Self& operator=(Self&&) noexcept = default;
};

} // namespace bzd::test
