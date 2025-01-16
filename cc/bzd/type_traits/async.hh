#pragma once

#include "cc/bzd/type_traits/is_same_class.hh"
#include "cc/bzd/type_traits/is_same_template.hh"
#include "cc/bzd/type_traits/is_trivially_copyable.hh"
#include "cc/bzd/type_traits/remove_cvref.hh"

namespace bzd::typeTraits {

enum class AsyncType
{
	task,
	generator
};

} // namespace bzd::typeTraits

namespace bzd::concepts {

template <class T>
concept async = requires(T& t) {
	{ t.type } -> sameClassAs<typeTraits::AsyncType>;
};

template <class T>
concept sync = !async<T>;

template <class T, class... Args>
concept asyncCallable = requires(T t, Args... args) {
	{ t(args...) } -> async;
};

} // namespace bzd::concepts
