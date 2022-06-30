#pragma once

#include "cc/bzd/meta/always_false.hh"
#include "cc/bzd/platform/types.hh"

// Provide traits of a function.
namespace bzd::typeTraits::impl {

template <class R, class... Args>
struct FunctionSignature
{
};

template <class... Args>
struct Function
{
	static_assert(bzd::meta::alwaysFalse<Args...>, "The object is not a function.");
};

template <class R, class... Args>
struct Function<R(Args...)>
{
	using Signature = FunctionSignature<R, Args...>;
	static constexpr Bool isNoexcept = false;
	static constexpr Bool isConst = false;
};

template <class R, class... Args>
struct Function<R(Args...) noexcept>
{
	using Signature = FunctionSignature<R, Args...>;
	static constexpr Bool isNoexcept = true;
	static constexpr Bool isConst = false;
};

template <class R, class... Args>
struct Function<R (*)(Args...)>
{
	using Signature = FunctionSignature<R, Args...>;
	static constexpr Bool isNoexcept = false;
	static constexpr Bool isConst = false;
};

template <class R, class... Args>
struct Function<R (*)(Args...) noexcept>
{
	using Signature = FunctionSignature<R, Args...>;
	static constexpr Bool isNoexcept = true;
	static constexpr Bool isConst = false;
};

template <class R, class U, class... Args>
struct Function<R (U::*)(Args...)>
{
	using Class = U;
	using Signature = FunctionSignature<R, Args...>;
	static constexpr Bool isNoexcept = false;
	static constexpr Bool isConst = false;
};

template <class R, class U, class... Args>
struct Function<R (U::*)(Args...) noexcept>
{
	using Class = U;
	using Signature = FunctionSignature<R, Args...>;
	static constexpr Bool isNoexcept = true;
	static constexpr Bool isConst = false;
};

template <class R, class U, class... Args>
struct Function<R (U::*)(Args...) const>
{
	using Class = U;
	using Signature = FunctionSignature<R, Args...>;
	static constexpr Bool isNoexcept = false;
	static constexpr Bool isConst = true;
};

template <class R, class U, class... Args>
struct Function<R (U::*)(Args...) const noexcept>
{
	using Class = U;
	using Signature = FunctionSignature<R, Args...>;
	static constexpr Bool isNoexcept = true;
	static constexpr Bool isConst = true;
};

} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T>
using Function = typename impl::Function<T>;
}
