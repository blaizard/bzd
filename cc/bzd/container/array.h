#pragma once

#include "cc/bzd/container/impl/span.h"
#include "cc/bzd/container/span.h"
#include "cc/bzd/container/storage/fixed.h"
#include "cc/bzd/platform/types.h"

namespace bzd {
/**
 * \brief A container that encapsulates fixed size arrays.
 *
 * The container combines the performance and accessibility of a C-style array
 * with the benefits of a standard container, such as knowing its own size,
 * supporting assignment, random access iterators, etc. Unlike a C-style array,
 * it doesn't decay to T* automatically.
 */
template <class T, SizeType N>
class Array : public impl::Span<T, impl::FixedStorage<T, N>>
{
protected:
	using Parent = impl::Span<T, impl::FixedStorage<T, N>>;
	using StorageType = typename Parent::StorageType;

public:
	template <class... Args>
	constexpr explicit Array(Args&&... args) noexcept : Parent{StorageType{args...}}
	{
	}

	constexpr Array() noexcept = default;

	/**
	 * \brief Returns the number of elements that the array can hold.
	 *
	 * \return Maximum number of element this array can hold.
	 */
	constexpr SizeType capacity() const noexcept { return N; }
};
} // namespace bzd
