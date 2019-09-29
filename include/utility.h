#pragma once

#include "include/types.h"
#include "include/type_traits/reference.h"

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
}
