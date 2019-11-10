#pragma once

#include "include/utility.h"
#include "include/type_traits/conditional.h"
#include "include/type_traits/is_reference.h"

namespace bzd
{
	namespace impl
	{
		// Forward declaration for the "friend" attribute
		template <class T, class E>
		class Expected;

		/**
		 * Internal class used to create an unexpected object type.
		 */
		template <class E>
		class Unexpected
		{
		public:
			constexpr Unexpected(const E& error) : error_(error) {}

		private:
			template <class A, class B>
			friend class bzd::impl::Expected;
			E error_;
		};

		template <class T, class E>
		class Expected
		{
		private:
			using Value = typename bzd::typeTraits::removeReference<T>::type;
			struct RefWrapper
			{
				constexpr RefWrapper(Value&& value) : value_(&value) {}
				constexpr Value& get() { return *value_; }
				constexpr const Value& get() const { return *value_; }
				Value* value_;
			};
			struct ValueWrapper
			{
				constexpr ValueWrapper(Value&& value) : value_(value) {}
				constexpr Value& get() { return value_; }
				constexpr const Value& get() const { return value_; }
				Value value_;
			};
			using ValueContainer = typename bzd::typeTraits::conditional<bzd::typeTraits::isReference<T>::value, RefWrapper, ValueWrapper>::type;

		public:
			template <class U>
			constexpr Expected(U&& value) : isError_(false), value_(bzd::forward<Value>(value))
			{
			}

			template <class U>
			constexpr Expected(impl::Unexpected<U>&& u) : isError_(true), error_(u.error_)
			{
			}

			// Move constructor
			constexpr Expected(Expected<T, E>&& e) : isError_(e.isError_)
			{
				if (isError_) error_ = bzd::move(e.error_);
				else value_ = bzd::move(e.value_);
			}

			~Expected()
			{
				if (isError_) error_.~E();
				else value_.~ValueContainer();
			}

			constexpr operator bool() const noexcept { return !isError_; }

			constexpr const E& error() const
			{
				// assert
				return error_;
			}

			constexpr const Value& operator*() const
			{
				// assert
				return value_.get();
			}

			constexpr Value& operator*()
			{
				// assert
				return value_.get();
			}

			constexpr const Value* operator->() const
			{
				// assert
				return &value_.get();
			}

			constexpr Value* operator->()
			{
				// assert
				return &value_.get();
			}

		protected:
			const bool isError_;
			union
			{
				ValueContainer value_;
				E error_;
			};
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
	}

	template <class T, class E>
	using Expected = impl::Expected<T, E>;

	template <class E>
	constexpr impl::Unexpected<typename bzd::decay<E>::type> makeUnexpected(E&& e)
	{
		return impl::Unexpected<typename bzd::decay<E>::type>(bzd::forward<E>(e));
	}
}
