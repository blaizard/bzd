#pragma once

#include "bzd/platform/types.h"
#include "bzd/type_traits/add_pointer.h"
#include "bzd/type_traits/decay.h"
#include "bzd/type_traits/invoke_result.h"
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
	using ReturnType = bzd::typeTraits::InvokeResult<T>;

public:
	Function(const FctPtrType callable) : callable_{callable} {}

	template <class... Args>
	ReturnType operator()(Args&&... args)
	{
		return callable_(bzd::forward<Args>(args)...);
	}

private:
	FctPtrType callable_;
};
} // namespace bzd::interface

namespace bzd {

template <class T>
class Function : public interface::Function<T>
{
private:
	using ReturnType = typename interface::Function<T>::ReturnType;

public:
	template <class Callable>
	Function(Callable&& instance) : interface::Function<T>{makeFunctionPointer(bzd::forward<Callable>(instance))}
	{
	}

private:
	template <class Callable, class... Args>
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
		return [](Args&&... args) -> ReturnType { return callable(bzd::forward<Args>(args)...); };
	}
};

} // namespace bzd
