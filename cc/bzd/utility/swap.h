#pragma once

#include "cc/bzd/utility/move.h"

namespace bzd {
template <class T>
constexpr void swap(T& t1, T& t2)
{
	T temp{bzd::move(t1)};
	t1 = bzd::move(t2);
	t2 = bzd::move(temp);
}
} // namespace bzd
