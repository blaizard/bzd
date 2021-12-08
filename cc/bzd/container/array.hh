#pragma once

#include "cc/bzd/container/impl/span.hh"
#include "cc/bzd/container/span.hh"
#include "cc/bzd/container/storage/fixed.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/utility/in_place.hh"

#include <initializer_list>

namespace bzd {
/// \brief A container that encapsulates fixed size arrays.
///
/// The container combines the performance and accessibility of a C-style array
/// with the benefits of a standard container, such as knowing its own size,
/// supporting assignment, random access iterators, etc. Unlike a C-style array,
/// it doesn't decay to T* automatically.
template <class T, SizeType N>
class Array : public impl::Span<T, impl::FixedStorage<T, N>>
{
public: // Traits.
	using Self = Array<T, N>;
	using Parent = impl::Span<T, impl::FixedStorage<T, N>>;
	using StorageType = typename Parent::StorageType;
	using ValueType = T;

public: // Constructors/assignments.
	constexpr Array() noexcept = default;
	constexpr Array(const Self&) noexcept = default;
	constexpr Self& operator=(const Self&) noexcept = default;
	constexpr Array(Self&&) noexcept = default;
	constexpr Self& operator=(Self&&) noexcept = default;

	template <class... Args>
	constexpr Array(InPlace, Args&&... args) noexcept : Parent{StorageType{inPlace, bzd::forward<Args>(args)...}}
	{
	}

	constexpr explicit Array(std::initializer_list<T> list) noexcept : Parent{StorageType{list.begin(), list.end()}} {}

public:
	/// \brief Returns the number of elements that the array can hold.
	///
	/// \return Maximum number of element this array can hold.
	static constexpr SizeType capacity() noexcept { return N; }
	static constexpr SizeType size() noexcept { return N; }
};
} // namespace bzd
