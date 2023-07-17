#pragma once

#include "cc/bzd/utility/pattern/to_stream/base.hh"
#include "cc/bzd/utility/pattern/to_string/byte.hh"

namespace bzd {

template <>
struct ToStream<bzd::Byte> : impl::ToStreamToString<bzd::Byte, 8u>
{
};

} // namespace bzd
