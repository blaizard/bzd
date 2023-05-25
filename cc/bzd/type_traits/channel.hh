#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/type_traits/range.hh"

namespace bzd::concepts {

namespace impl {
template <class T>
concept readerAsyncReturn = concepts::async<T> && concepts::inputRange<typename T::Value>;
}

template <class T>
concept readerAsync = requires(T&& t) {
						  {
							  t.reader()
							  } -> impl::readerAsyncReturn;
					  };

} // namespace bzd::concepts
