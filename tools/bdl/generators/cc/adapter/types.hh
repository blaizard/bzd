#pragma once

#include "cc/bzd/container/function_ref.hh"
#include "cc/bzd/container/result.hh"
#include "cc/bzd/container/span.hh"
#include "cc/bzd/container/vector.hh"
#include "cc/bzd/core/error.hh"
#include "cc/bzd/meta/string_literal.hh"
#include "cc/bzd/type_traits/derived_from.hh"
#include "cc/bzd/type_traits/function.hh"
#include "cc/bzd/type_traits/is_same.hh"

namespace bzd {
/// Interfaces accessor.
template <bzd::meta::StringLiteral fqn>
struct Interface;

namespace impl {
/// Helper to validate and get the implementation method from an interface.
///
/// \param obj The current object containing the interface (usually 'this').
/// \param interface The member pointer of the interface function.
/// \param impl The member pointer of the implementation function.
/// \return A point of the implementation object.
template <class T, class Interface, class Impl>
constexpr auto* getMethod(T* obj, Interface interface, Impl impl) noexcept
{
	using TraitsInterface = bzd::typeTraits::Function<decltype(interface)>;
	using TraitsImpl = bzd::typeTraits::Function<decltype(impl)>;
	static_assert(bzd::typeTraits::isSame<typename TraitsInterface::Signature, typename TraitsImpl::Signature>,
				  "Signature doesn't match its interface.");
	static_assert(TraitsInterface::isNoexcept && TraitsImpl::isNoexcept, "Interface must have the same exception guaranty.");
	return static_cast<typename TraitsImpl::Class*>(obj);
}

/// Helper to get the interface from the implementation.
///
/// \param impl The implementation object.
/// \{
template<template<class> class T, class Impl>
constexpr auto& getInterface(Impl& impl) { return static_cast<T<Impl>&>(impl); }

template<class T, class Impl>
constexpr auto& getInterface(Impl& impl) { return static_cast<T&>(impl); }
/// \}

} // namespace impl

} // namespace bzd
