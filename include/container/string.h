#pragma once

#include "include/types.h"
#include "include/container/span.h"
#include "include/container/string_view.h"
#include "include/type_traits/const_volatile.h"

namespace bzd
{
	namespace impl
	{
		template <class T, class Impl>
		class String : public Impl
		{
		protected:
			using Parent = Impl;
			using Impl::size_;
			using Impl::data_;

			using StringView = bzd::impl::StringView<T, bzd::Span<typename bzd::typeTraits::addConst<T>::type>>;

		public:
			template <class... Args>
			constexpr explicit String(const SizeType capacity, Args&&... args)
					: Impl(args...), capacity_(capacity)
			{
			}

			// Converter
			constexpr SizeType append(const StringView& str) noexcept { return append(str.data(), str.size()); }
			constexpr SizeType append(const T c) noexcept { return append(&c, 1); }
			constexpr SizeType append(const T* data, const SizeType n) noexcept
			{
				// Handles overflows
				const SizeType sizeLeft = capacity_ - size_ - 1;
				const SizeType actualN = (sizeLeft < n) ? sizeLeft : n;
				memcpy(&Parent::at(size_), data, actualN);
				size_ += actualN;
				Parent::at(size_) = '\0';

				return actualN;
			}

			constexpr SizeType capacity() const noexcept
			{
				return capacity_ - 1;
			}

			constexpr void clear() noexcept
			{
				resize(0);
			}

			constexpr void resize(const SizeType n) noexcept
			{
				size_ = (n < capacity_ - 1) ? n : capacity_ - 1;
				Parent::at(size_) = '\0';
			}

			template <class U>
			constexpr String<T, Impl>& operator+=(const U& data) noexcept
			{
				append(data);
				return *this;
			}

			template <class U>
			constexpr String<T, Impl>& operator=(const U& data) noexcept
			{
				clear();
				return operator+=(data);
			}

		public:
			const SizeType capacity_;
		};
	}

	namespace interface
	{
		using String = impl::String<char, Span<char>>;
	}

	template <SizeType N>
	class String : public interface::String
	{
	public:
		constexpr String() : interface::String(N + 1, data_, 0) { data_[0] = '\0'; }
		constexpr explicit String(const interface::String::StringView& str) : String() { append(str); }

		template <class T>
		constexpr interface::String& operator=(const T& data) noexcept
		{
			return interface::String::operator=(data);
		}

	protected:
		interface::String::DataType data_[N + 1];
	};
}
