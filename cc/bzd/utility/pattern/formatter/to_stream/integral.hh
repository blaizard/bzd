#pragma once

#include "cc/bzd/utility/pattern/formatter/to_stream/base.hh"
#include "cc/bzd/utility/pattern/formatter/to_string/integral.hh"

namespace bzd {

template <concepts::integral T>
struct FormatterAsync<T> : impl::FormatterAsyncToFormatterWithMetadata<T, 80u>
{
};

} // namespace bzd
