#pragma once

#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/utility/forward.hh"

namespace bzd::concepts {
template <class T, class... Args>
concept invocable = requires(T&& t, Args&&... args)
{
	t(bzd::forward<Args>(args)...);
};

template <class T, class R, class... Args>
concept invocableR = requires(T&& t, Args&&... args)
{
	{
		t(bzd::forward<Args>(args)...)
		} -> sameAs<R>;
};
} // namespace bzd::concepts
