#pragma once

#include "include/types.h"
#include "include/container/span.h"
#include "include/container/string_view.h"

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

			using StringView = ::bzd::impl::StringView<T, ConstSpan<T>>;

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

			void clear() noexcept
			{
				resize(0);
			}

			void resize(const SizeType n) noexcept
			{
				size_ = (n < capacity_ - 1) ? n : capacity_ - 1;
				Parent::at(size_) = '\0';
			}

			template <class U>
			String<T, Impl>& operator+=(const U& data) noexcept
			{
				append(data);
				return *this;
			}

			template <class U>
			String<T, Impl>& operator=(const U& data) noexcept
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
		interface::String& operator=(const T& data) noexcept
		{
			return interface::String::operator=(data);
		}

	protected:
		interface::String::DataType data_[N + 1];
	};
}
