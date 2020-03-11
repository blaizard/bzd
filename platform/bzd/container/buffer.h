#pragma once

#include "bzd/container/span.h"
#include "bzd/container/string_view.h"

namespace bzd {
namespace impl {
template <class T>
class Buffer : public bzd::Span<T>
{
protected:
	using typename bzd::Span<T>::IsConst;

public:
	using bzd::Span<T>::Span;

	template <class Q = IsConst, bzd::typeTraits::EnableIf<Q::value, void>* = nullptr>
	constexpr Buffer(const bzd::StringView& str) : bzd::Span<T>(reinterpret_cast<T*>(str.data()), str.size())
	{
	}

	template <class Q, bzd::typeTraits::EnableIf<sizeof(Q) == 1, void>* = nullptr>
	constexpr Buffer(Q* data, const SizeType size) : bzd::Span<T>(reinterpret_cast<T*>(data), size)
	{
	}
};
} // namespace impl

using Buffer = impl::Buffer<bzd::UInt8Type>;
using ConstBuffer = impl::Buffer<const bzd::UInt8Type>;

} // namespace bzd
