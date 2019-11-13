#pragma once

#include "bzd/types.h"
#include "bzd/type_traits/is_array.h"
#include "bzd/type_traits/is_function.h"
#include "bzd/type_traits/is_lvalue_reference.h"
#include "bzd/type_traits/add_pointer.h"
#include "bzd/type_traits/remove_extent.h"
#include "bzd/type_traits/remove_reference.h"
#include "bzd/type_traits/remove_cv.h"
#include "bzd/type_traits/enable_if.h"
#include "bzd/type_traits/conditional.h"

namespace bzd
{
	/**
	 * Singleton class pattern
	 */
	template<class T>
	class SingletonImpl
	{
	public:
		static T& getInstance()
		{
			static T instance;
			return instance;
		}
		SingletonImpl(SingletonImpl const&) = delete;
		void operator=(SingletonImpl const&) = delete;

	protected:
		SingletonImpl() = default;
	};

	template<class T>
	class SingletonThreadLocalImpl
	{
	public:
		static T& getInstance()
		{
			static thread_local T instance;
			return instance;
		}
		SingletonThreadLocalImpl(SingletonThreadLocalImpl const&) = delete;
		void operator=(SingletonThreadLocalImpl const&) = delete;

	protected:
		SingletonThreadLocalImpl() = default;
	};

	template <class T, class M>
	static inline constexpr IntPtrType offsetOf(const M T::*member)
	{
		return reinterpret_cast<IntPtrType>(&(reinterpret_cast<T*>(0)->*member));
	}

	template <class T, class M>
	static inline constexpr T* containerOf(M* ptr, const M T::*member)
	{
		return reinterpret_cast<T*>(reinterpret_cast<IntPtrType>(ptr) - offsetOf(member));
	}

	// memcpy

	constexpr void memcpy(char* dest, const char* src, const SizeType size)
	{
		for (SizeType i = 0; i < size; ++i)
		{
			dest[i] = src[i];
		}
	}

	// forward

	template <class T>
	constexpr T&& forward(typename typeTraits::removeReference<T>::type& t) noexcept
	{
		return static_cast<T&&>(t);
	}
	template <class T>
	constexpr T&& forward(typename typeTraits::removeReference<T>::type&& t) noexcept
	{
		static_assert(!bzd::typeTraits::isLValueReference<T>::value, "template argument substituting T is an lvalue reference type");
		return static_cast<T&&>(t);
	}

	// move

	template <class T>
	typename bzd::typeTraits::removeReference<T>::type&& move(T&& arg)
	{
		return static_cast<typename bzd::typeTraits::removeReference<T>::type&&>(arg);
	}

	// swap

	template <class T>
	constexpr void swap(T& t1, T& t2)
	{
		T temp{bzd::move(t1)};
		t1 = bzd::move(t2);
		t2 = bzd::move(temp);
	}

	// min

	template <class T>
	constexpr T min(const T& a, const T& b) noexcept
	{
		return (a < b) ? a : b;
	}

	// max

	template <class T>
	constexpr T max(const T& a, const T& b) noexcept
	{
		return (a > b) ? a : b;
	}

	// decay

	template <class T>
	struct decay
	{
	private:
		typedef typename bzd::typeTraits::removeReference<T>::type U;
	public:
		typedef typename bzd::typeTraits::conditional<bzd::typeTraits::isArray<U>::value, typename bzd::typeTraits::removeExtent<U>::type*,
			typename bzd::typeTraits::conditional<bzd::typeTraits::isFunction<U>::value, typename bzd::typeTraits::addPointer<U>::type, typename bzd::typeTraits::removeCV<U>::type
        	>::type
    	>::type type;
	};

	// alignedStorage

	template <SizeType Len, SizeType Align>
	struct alignedStorage
	{
		struct type
		{
    		alignas(Align) unsigned char data[Len];
    	};
	};
}
