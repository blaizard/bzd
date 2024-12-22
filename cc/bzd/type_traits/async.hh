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

template <class T>
using AsyncValue = typename typeTraits::RemoveCVRef<T>::Value;

template <class T>
using AsyncError = typename typeTraits::RemoveCVRef<T>::Error;

} // namespace bzd::typeTraits

namespace bzd::concepts {

template <class T>
concept async = requires(T& t) {
	{
		t.type
	} -> sameClassAs<typeTraits::AsyncType>;
};
template <class T>
concept sync = !async<T>;
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

template <class T>
concept asyncGeneratorByteCopyable = concepts::asyncGenerator<T> && sizeof(typeTraits::RemoveCVRef<typeTraits::AsyncValue<T>>) == 1u &&
									 concepts::triviallyCopyable<typeTraits::RemoveCVRef<typeTraits::AsyncValue<T>>>;

} // namespace bzd::concepts
