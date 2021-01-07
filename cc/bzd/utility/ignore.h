#pragma once

namespace bzd::utility::impl {
/**
 * An object of unspecified type such that any value can be assigned to it with no effect.
 */
struct Ignore
{
	template <class T>
	constexpr const Ignore& operator=(T&&) const noexcept
	{
		return *this;
	}
};
} // namespace bzd::utility::impl

namespace bzd {
constexpr utility::impl::Ignore ignore{};
}
