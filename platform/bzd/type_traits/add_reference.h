#pragma once

namespace bzd { namespace typeTraits { namespace impl {
template <class T>
struct AddReference
{
	typedef T& type;
};

template <class T>
struct AddReference<T&>
{
	typedef T& type;
};

template <class T>
struct AddReference<T&&>
{
	typedef T&& type;
};

template <>
struct AddReference<void>
{
	typedef void type;
};
template <>
struct AddReference<const void>
{
	typedef const void type;
};
template <>
struct AddReference<const volatile void>
{
	typedef const volatile void type;
};
template <>
struct AddReference<volatile void>
{
	typedef volatile void type;
};
}

template <class T>
using AddReference = typename impl::AddReference<T>::type;

}} // namespace bzd::typeTraits
