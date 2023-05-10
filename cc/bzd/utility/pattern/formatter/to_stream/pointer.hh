#pragma once

#include "cc/bzd/utility/pattern/formatter/to_stream/base.hh"
#include "cc/bzd/utility/pattern/formatter/to_string/pointer.hh"

namespace bzd {

template <class T>
requires(concepts::pointer<T> && !concepts::convertible<T, bzd::StringView>)
struct FormatterAsync<T> : impl::FormatterAsyncToFormatter<T, 20u>
{
};

} // namespace bzd
