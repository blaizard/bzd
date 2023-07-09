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

} // namespace bzd::concepts
