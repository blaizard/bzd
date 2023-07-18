#pragma once

#include "cc/bzd/type_traits/is_same_class.hh"
#include "cc/bzd/type_traits/is_same_template.hh"

namespace bzd::typeTraits {

enum class AsyncType
{
	task,
	generator
};

}

namespace bzd::concepts {

template <class T>
concept async = requires(T t) {
	{
		t.type
	} -> sameClassAs<typeTraits::AsyncType>;
};
template <class T>
concept asyncTask = async<T> && (typeTraits::RemoveCVRef<T>::type == typeTraits::AsyncType::task);
template <class T>
concept asyncGenerator = async<T> && (typeTraits::RemoveCVRef<T>::type == typeTraits::AsyncType::generator);

template <class T, class... Args>
concept asyncCallable = requires(T t, Args... args) {
	{
		t(args...)
	} -> async;
};

} // namespace bzd::concepts
