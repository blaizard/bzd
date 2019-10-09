#pragma once

#include "include/utility.h"
#include "include/type_traits/utils.h"

namespace bzd
{
	// Forward declaration for the "friend" attribute
	template <class T, class E>
	class Expected;

	namespace impl
	{
		/**
		 * Internal class used to create an unexpected object type.
		 */
		template <class E>
		class Unexpected
		{
		public:
			constexpr Unexpected(E&& error) : error_(error) {}

		private:
			template <class A, class B>
			friend class bzd::Expected;
			E error_;
		};
	}

	template <class T, class E>
	class Expected
	{
	public:
		template <class U>
		constexpr Expected(U&& value) : isError_(false), value_(value)
		{
		}

		template <class U>
		constexpr Expected(impl::Unexpected<U>&& u) : isError_(true), error_(u.error_)
		{
		}

		constexpr operator bool() const noexcept { return !isError_; }

		constexpr const E& error() const
		{
			// assert
			return error_;
		}

		constexpr const T& operator*() const
		{
			// assert
			return value_;
		}

		constexpr T& operator*()
		{
			// assert
			return value_;
		}

		constexpr const T* operator->() const
		{
			// assert
			return &value_;
		}

		constexpr T* operator->()
		{
			// assert
			return &value_;
		}

	protected:
		const bool isError_;
		T value_;
		E error_;
	};

	template <class E>
	class Expected<void, E> : private Expected<void*, E>
	{
	public:
		using Expected<void*, E>::Expected;
		using Expected<void*, E>::operator bool;
		using Expected<void*, E>::error;

		constexpr Expected() : Expected<void*, E>(nullptr) {}
	};

	template <class E>
	constexpr impl::Unexpected<typename bzd::decay<E>::type> makeUnexpected(E&& e)
	{
		return impl::Unexpected<typename bzd::decay<E>::type>(bzd::forward<E>(e));
	}
}
