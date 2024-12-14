#pragma once

#include <memory>

namespace bzd {
template <class T>
constexpr void destroyAt(T* p)
{
	std::destroy_at(p);
}
} // namespace bzd
