#pragma once

#include "bzd/platform/types.h"
#include "bzd/type_traits/add_pointer.h"
#include "bzd/type_traits/decay.h"
#include "bzd/utility/forward.h"

#include <new> // Used only for placement new

namespace bzd::interface {

/**
 * Provides a typed-erased callable with static allocation.
 *
 * \note There is an important limitation, 2 callable of the same type
 * cannot be executed concurently. But this limitation is diminished
 * when used with lambdas as their type uniticy is guaranteed see:
 * [C++11: 5.1.2/3]: The type of the lambda-expression (which is also
 * the type of the closure object) is a unique, unnamed non-union class
 * type — called the closure type — whose properties are described below.
 * This class type is not an aggregate (8.5.1). The closure type is declared
 * in the smallest block scope, class scope, or namespace scope that contains
 * the corresponding lambda-expression.
 */
template <class T>
class Function
{
protected:
	using FctPtrType = bzd::typeTraits::AddPointer<T>;

public:
	Function(const FctPtrType callable) : callable_{callable} {}

	void operator()() { callable_(); }

private:
	FctPtrType callable_;
};
} // namespace bzd::interface

namespace bzd {

template <class T>
class Function : public interface::Function<T>
{
private:
	using FctPtrType = typename interface::Function<T>::FctPtrType;

public:
	template <class Callable>
	Function(Callable&& instance) : interface::Function<T>{makeFunctionPointer(bzd::forward<Callable>(instance))}
	{
	}

private:
	template <class Callable>
	static FctPtrType makeFunctionPointer(Callable&& instance)
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
		return []() { callable(); };
	}
};

} // namespace bzd
