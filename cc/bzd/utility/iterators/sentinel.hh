#pragma once

#include "cc/bzd/platform/types.hh"

namespace bzd::iterator {

template <class Iterator>
class Sentinel
{
public:
	constexpr Bool operator==(const Iterator& it) const noexcept { return (it == *this); }
	constexpr Bool operator!=(const Iterator& it) const noexcept { return (it != *this); }
};

} // namespace bzd::iterator
