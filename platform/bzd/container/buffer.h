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

	template <class Q = IsConst, typename bzd::typeTraits::enableIf<Q::value, void>::type* = nullptr>
	constexpr Buffer(const bzd::StringView& str) : bzd::Span<T>(reinterpret_cast<T*>(str.data()), str.size()) {}
};
}

using Buffer = impl::Buffer<bzd::UInt8Type>;
using ConstBuffer = impl::Buffer<const bzd::UInt8Type>;

} // namespace bzd
