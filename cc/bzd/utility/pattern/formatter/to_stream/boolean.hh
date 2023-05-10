#pragma once

#include "cc/bzd/utility/pattern/formatter/to_stream/base.hh"
#include "cc/bzd/utility/pattern/formatter/to_string/boolean.hh"

namespace bzd {

template <>
struct FormatterAsync<bzd::Bool> : impl::FormatterAsyncToFormatter<bzd::Bool, 10u>
{
};

} // namespace bzd
