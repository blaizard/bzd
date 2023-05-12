#pragma once

#include "cc/bzd/utility/pattern/to_stream/base.hh"
#include "cc/bzd/utility/pattern/to_string/floating_point.hh"

namespace bzd {

template <concepts::floatingPoint T>
struct ToStream<T> : impl::ToStreamToString<T, 80u>
{
};

} // namespace bzd
