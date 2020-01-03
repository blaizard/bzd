#pragma once

namespace bzd { namespace typeTraits {
template <class T>
struct addReference
{
	typedef T &type;
};

template <class T>
struct addReference<T &>
{
	typedef T &type;
};

template <class T>
struct addReference<T &&>
{
	typedef T &&type;
};

template <>
struct addReference<void>
{
	typedef void type;
};
template <>
struct addReference<const void>
{
	typedef const void type;
};
template <>
struct addReference<const volatile void>
{
	typedef const volatile void type;
};
template <>
struct addReference<volatile void>
{
	typedef volatile void type;
};
}}	// namespace bzd::typeTraits
