#pragma once

#include "include/types.h"
#include "include/container/span.h"

namespace bzd
{
	namespace impl
	{
		template <class T, class Impl>
		class BasicString : public Impl
		{
		protected:
			using Impl::size_;
			using Impl::data_;

		public:
			template <class... Args>
			BasicString(const SizeType capacity, Args&&... args)
					: Impl(args...), capacity_(capacity)
			{
			}

			// Converter
			void append(const char* str) noexcept { append(str, strlen(str)); }
			void append(const char c) noexcept { append(&c, 1); }
			void append(const char* str, const SizeType n) noexcept
			{
				// Handles overflows
				const SizeType sizeLeft = capacity_ - size_ - 1;
				const SizeType actualN = (sizeLeft < n) ? sizeLeft : n;
				memcpy(&data_[size_], str, actualN);
				size_ += actualN;
				data_[size_] = '\0';
			}

			SizeType capacity() const noexcept
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
				data_[size_] = '\0';
			}

			template <class U>
			BasicString<T, Impl>& operator+=(const U& data) noexcept
			{
				append(data);
				return *this;
			}

			template <class U>
			BasicString<T, Impl>& operator=(const U& data) noexcept
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
		using String = impl::BasicString<char, Span<char, /*IsSizeConst*/false>>;
	}

	template <SizeType N>
	class String : public interface::String
	{
	public:
		String() : interface::String(N + 1, data_, 0) {}
		String(const char* str) : String() { append(str); }

	protected:
		char data_[N];
	};
}
