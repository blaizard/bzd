#pragma once

#include "cc/bzd/utility/pattern/to_stream/base.hh"
#include "cc/bzd/utility/pattern/to_string/boolean.hh"

namespace bzd {

template <>
struct ToStream<bzd::Bool> : impl::ToStreamToString<bzd::Bool, 10u>
{
};

} // namespace bzd
