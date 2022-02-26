#pragma once

#include "cc/bzd/container/function_ref.hh"
#include "cc/bzd/container/result.hh"
#include "cc/bzd/container/span.hh"
#include "cc/bzd/container/vector.hh"

#include <type_traits>

/*
#define STATIC_ASSERT_MEMBER_METHOD(class, member, signature)                                       \
\
template<typename, typename T> \
struct has_##member { \
	static_assert( \
		std::integral_constant<T, false>::value, \
		"Second template parameter needs to be of function type."); \
}; \
\
template<typename C, typename Ret, typename... Args> \
struct has_##member<C, Ret(Args...)> { \
private: \
	template<typename T> \
	static constexpr auto check(T*) \
	-> typename \
		std::is_same<decltype( std::declval<T>().##member( std::declval<Args>()... ) ),Ret>::type; \
\
	template<typename> \
	static constexpr std::false_type check(...); \
\
	typedef decltype(check<C>(0)) type; \
\
public: \
	static constexpr bool value = type::value; \
}; \
\
static_assert(has_##member<class, signature>::value, "Function signature not correct.")
*/
