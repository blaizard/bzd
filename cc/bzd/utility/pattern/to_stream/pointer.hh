#pragma once

#include "cc/bzd/utility/pattern/to_stream/base.hh"
#include "cc/bzd/utility/pattern/to_string/pointer.hh"

namespace bzd {

template <class T>
requires(concepts::pointer<T> && !concepts::convertible<T, bzd::StringView>)
struct ToStream<T> : impl::ToStreamToString<T, 20u>
{
};

} // namespace bzd
