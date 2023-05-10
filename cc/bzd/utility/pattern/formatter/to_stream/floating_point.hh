#pragma once

#include "cc/bzd/utility/pattern/formatter/to_stream/base.hh"
#include "cc/bzd/utility/pattern/formatter/to_string/floating_point.hh"

namespace bzd {

template <concepts::floatingPoint T>
struct FormatterAsync<T> : impl::FormatterAsyncToFormatterWithMetadata<T, 80u>
{
};

} // namespace bzd
