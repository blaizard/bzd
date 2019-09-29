#pragma once

#include "include/container/iostream.h"
#include "include/container/string.h"
#include "include/container/string_view.h"

namespace bzd
{
	namespace impl
	{
		class StringStream : public bzd::OStream<char>
		{
		protected:
			StringStream(bzd::interface::String& str)
					: string_(str)
			{
			}

		public:
			SizeType write(const bzd::StringView data)
			{
				return write(static_cast<const bzd::ConstSpan<char>>(data));
			}

			SizeType write(const bzd::ConstSpan<char> data) override
			{
				return string_.append(data.data(), data.size());
			}

		protected:
			bzd::interface::String& string_;
		};
	}

	namespace interface
	{
		using StringStream = bzd::impl::StringStream;
	}

	template <SizeType N>
	class StringStream : public bzd::interface::StringStream
	{
	public:
		StringStream()
				: bzd::interface::StringStream(container_)
				, container_()
		{
		}

	private:
		bzd::String<N> container_;
	};
}
