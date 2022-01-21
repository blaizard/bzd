#pragma once

#include "cc/bzd/type_traits/integral_constant.hh"
#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/type_traits/remove_cvref.hh"

namespace bzd::typeTraits::impl {
template <class T>
struct IsIntegral
	: IntegralConstant<bool,
					   isSame<unsigned char, RemoveCVRef<T>> || isSame<signed char, RemoveCVRef<T>> || isSame<char, RemoveCVRef<T>> ||
						   isSame<unsigned short, RemoveCVRef<T>> || isSame<short, RemoveCVRef<T>> ||
						   isSame<unsigned int, RemoveCVRef<T>> || isSame<int, RemoveCVRef<T>> || isSame<unsigned long, RemoveCVRef<T>> ||
						   isSame<long, RemoveCVRef<T>> || isSame<unsigned long long, RemoveCVRef<T>> ||
						   isSame<long long, RemoveCVRef<T>> || isSame<bool, RemoveCVRef<T>>>
{
};
} // namespace bzd::typeTraits::impl

namespace bzd::typeTraits {
template <class T>
using IsIntegral = typename impl::IsIntegral<T>;

template <class T>
constexpr bool isIntegral = IsIntegral<T>::value;

} // namespace bzd::typeTraits

namespace bzd {
template <class T>
concept integral = typeTraits::isIntegral<T>;
}
