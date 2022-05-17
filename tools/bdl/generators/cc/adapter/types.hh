#pragma once

#include "cc/bzd/type_traits/function.hh"
#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/container/function_ref.hh"
#include "cc/bzd/container/result.hh"
#include "cc/bzd/container/span.hh"
#include "cc/bzd/container/vector.hh"
#include "cc/bzd/meta/string_literal.hh"

namespace bzd
{
/// Interfaces accessor.
template <bzd::meta::StringLiteral fqn>
struct Interface;
}
