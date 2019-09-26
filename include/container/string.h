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
			using Impl::size_;
			using Impl::data_;
			using StringView = ::bzd::impl::StringView<T, Span<T, /*IsDataConst*/true>>;
			using SpanType = Span<T>;

		public:
			template <class... Args>
			explicit String(const SizeType capacity, Args&&... args)
					: Impl(args...), capacity_(capacity)
			{
			}

			// Converter
			void append(const StringView& str) noexcept { append(str.data(), str.size()); }
			void append(const T c) noexcept { append(&c, 1); }
			void append(const T* data, const SizeType n) noexcept
			{
				// Handles overflows
				const SizeType sizeLeft = capacity_ - size_ - 1;
				const SizeType actualN = (sizeLeft < n) ? sizeLeft : n;
				memcpy(&data_[size_], data, actualN);
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

			SpanType postSpan() const noexcept
			{
				return SpanType(&data_[size_], capacity_ - size_);
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
		String() : interface::String(N + 1, data_, 0) { data_[0] = '\0'; }
		explicit String(const interface::String::StringView& str) : String() { append(str); }

		template <class T>
		interface::String& operator=(const T& data) noexcept
		{
			return interface::String::operator=(data);
		}

	protected:
		interface::String::DataType data_[N + 1];
	};
}
