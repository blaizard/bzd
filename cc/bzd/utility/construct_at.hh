#pragma once

#include "cc/bzd/utility/forward.hh"

#include <memory>

namespace bzd {
template <class T, class... Args>
constexpr T* constructAt(T* p, Args&&... args)
{
	return std::construct_at(p, bzd::forward<Args>(args)...);
}
} // namespace bzd
