#pragma once

#include "cc/bzd/type_traits/invocable.hh"
#include "cc/bzd/type_traits/is_convertible.hh"
#include "cc/bzd/utility/forward.hh"

namespace bzd::concepts {

template <class T, class... Args>
concept predicate = requires(T&& t, Args&&... args) {
	{ t(bzd::forward<Args>(args)...) } -> convertible<Bool>;
};

} // namespace bzd::concepts
