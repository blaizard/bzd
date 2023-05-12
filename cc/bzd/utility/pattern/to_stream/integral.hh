#pragma once

#include "cc/bzd/utility/pattern/to_stream/base.hh"
#include "cc/bzd/utility/pattern/to_string/integral.hh"

namespace bzd {

template <concepts::integral T>
struct ToStream<T> : impl::ToStreamToString<T, 80u>
{
};

} // namespace bzd
