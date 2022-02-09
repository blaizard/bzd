#pragma once

#include "cc/bzd/container/iterator/begin.hh"
#include "cc/bzd/container/iterator/end.hh"

namespace bzd::concepts {
template <class T>
concept range = requires(T t)
{
	bzd::begin(t);
	bzd::end(t);
};
} // namespace bzd::concepts
