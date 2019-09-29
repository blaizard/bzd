#pragma once

#include "include/types.h"
#include "include/container/string_view.h"

namespace bzd
{
	namespace impl
	{
		template <class T>
		class OStream
		{
		protected:
			OStream() = default;

		public:
			virtual SizeType write(const ConstSpan<T>& data) noexcept = 0;
		};

		template <class T>
		class IStream
		{
		protected:
			IStream() = default;

		public:
			virtual SizeType read(Span<T>& data) noexcept = 0;
		};

		template <class T>
		class IOStream : public IStream<T>, public OStream<T>
		{
		};
	}

	using OStream = impl::OStream<char>;
	using IStream = impl::IStream<char>;
	using IOStream = impl::IOStream<char>;
	
}
