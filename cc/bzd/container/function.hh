#pragma once

#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/add_pointer.hh"
#include "cc/bzd/type_traits/decay.hh"
#include "cc/bzd/utility/forward.hh"

#include <new> // Used only for placement new

namespace bzd::impl {
/// Forward declaration for friend
template <typename Tag, class F, class... Args>
class FunctionView;
} // namespace bzd::impl

namespace bzd::interface {

/// Provides a typed-erased callable with static allocation.
///
/// \note There is an important limitation, 2 callable of the same type
/// cannot be executed concurently. But this limitation is diminished
/// when used with lambdas as their type uniticy is guaranteed see:
/// [C++11: 5.1.2/3]: The type of the lambda-expression (which is also
/// the type of the closure object) is a unique, unnamed non-union class
/// type — called the closure type — whose properties are described below.
/// This class type is not an aggregate (8.5.1). The closure type is declared
/// in the smallest block scope, class scope, or namespace scope that contains
/// the corresponding lambda-expression.
template <class F, class... Args>
class Function
{
protected:
	using FctPtrType = bzd::typeTraits::AddPointer<F(Args...)>;
	using ReturnType = F;

public:
	Function(const FctPtrType callable) : callable_{callable} {}

	template <class... Params> // Needed for perfect forwarding
	ReturnType operator()(Params&&... args) const
	{
		return callable_(bzd::forward<Params>(args)...);
	}

private:
	template <typename Tag, class A, class... B>
	friend class ::bzd::impl::FunctionView;

	FctPtrType callable_;
};
} // namespace bzd::interface

namespace bzd::impl {

template <typename Tag, class F, class... Args>
class Function : public interface::Function<F, Args...>
{
private:
	using ReturnType = typename interface::Function<F, Args...>::ReturnType;

public:
	template <class Callable>
	Function(Callable&& instance) : interface::Function<F, Args...>{makeFunctionPointer(bzd::forward<Callable>(instance))}
	{
	}

private:
	template <class Callable>
	static auto makeFunctionPointer(Callable&& instance)
	{
		using CallableStorageType = bzd::typeTraits::Decay<Callable>;
		static bool used = false;
		static CallableStorageType callable{bzd::forward<Callable>(instance)};
		if (used)
		{
			callable.~CallableStorageType();
			new (&callable) CallableStorageType(bzd::forward<Callable>(instance));
		}
		used = true;
		return [](Args... args) -> ReturnType { return callable(bzd::forward<Args>(args)...); };
	}
};

struct FunctionTag
{
};

} // namespace bzd::impl

namespace bzd {

template <class, typename Tag = impl::FunctionTag>
class Function;
template <class F, class... ArgTypes>
class Function<F(ArgTypes...)> : public impl::Function<impl::FunctionTag, F, ArgTypes...>
{
};
template <typename Tag, class F, class... ArgTypes>
class Function<F(ArgTypes...), Tag> : public impl::Function<Tag, F, ArgTypes...>
{
};

} // namespace bzd
