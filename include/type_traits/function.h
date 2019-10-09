#pragma once

#include "include/type_traits/integral_constant.h"

namespace bzd
{
	namespace typeTraits
	{
		// isFunction

		// Primary template
		template <class> struct isFunction : falseType {};
 
		// Specialization for regular functions
		template <class Ret, class... Args> struct isFunction<Ret(Args...)> : trueType {};
 
		// Specialization for function types that have cv-qualifiers
		template <class Ret, class... Args> struct isFunction<Ret(Args...) const> : trueType {};
		template <class Ret, class... Args> struct isFunction<Ret(Args...) volatile> : trueType {};
		template <class Ret, class... Args> struct isFunction<Ret(Args...) const volatile> : trueType {};
		
		// Specialization for function types that have ref-qualifiers
		template <class Ret, class... Args> struct isFunction<Ret(Args...) &> : trueType {};
		template <class Ret, class... Args> struct isFunction<Ret(Args...) const &> : trueType {};
		template <class Ret, class... Args> struct isFunction<Ret(Args...) volatile &> : trueType {};
		template <class Ret, class... Args> struct isFunction<Ret(Args...) const volatile &> : trueType {};
		template <class Ret, class... Args> struct isFunction<Ret(Args...) &&> : trueType {};
		template <class Ret, class... Args> struct isFunction<Ret(Args...) const &&> : trueType {};
		template <class Ret, class... Args> struct isFunction<Ret(Args...) volatile &&> : trueType {};
		template <class Ret, class... Args> struct isFunction<Ret(Args...) const volatile &&> : trueType {};
	}
}
