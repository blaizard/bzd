#pragma once

#include <memory>

namespace bzd {
template <class T>
constexpr T* addressOf(T& arg)
{
	return std::addressof(arg);
}
template <class T>
constexpr const T* addressOf(const T&& arg)
{
	return std::addressof(arg);
}
} // namespace bzd
