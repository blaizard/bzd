#pragma once

#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/enable_if.hh"

/// Helper macros to provide concept-like functionality before c++20

/// Defines a new concept.
#define CONCEPT inline constexpr bzd::BoolType

/// Mimics the requires keyword using SFINAE technics
#define REQUIRES(...) typename bzd::typeTraits::EnableIf<__VA_ARGS__, bzd::BoolType> = true
#define REQUIRES_SPECIALIZATION(...) bzd::typeTraits::EnableIf<__VA_ARGS__, void>
