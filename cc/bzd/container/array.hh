#pragma once

#include "cc/bzd/container/impl/span.hh"
#include "cc/bzd/container/span.hh"
#include "cc/bzd/container/storage/fixed.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/utility/in_place.hh"

#include <initializer_list>

namespace bzd {
/// \brief A container that encapsulates fixed size arrays.
///
/// The container combines the performance and accessibility of a C-style array
/// with the benefits of a standard container, such as knowing its own size,
/// supporting assignment, random access iterators, etc. Unlike a C-style array,
/// it doesn't decay to T* automatically.
template <class T, Size maxCapacity>
class Array : public impl::Span<T, impl::FixedStorage<T, maxCapacity>>
{
public: // Traits.
	using Self = Array<T, maxCapacity>;
	using Parent = impl::Span<T, impl::FixedStorage<T, maxCapacity>>;
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

	constexpr explicit Array(std::initializer_list<T> list) noexcept
	requires(!concepts::reference<T>)
		: Parent{StorageType{list.begin(), list.end()}}
	{
	}

	template <Size index>
	constexpr auto& get() noexcept
	{
		return (*this)[index];
	}

	template <Size index>
	constexpr const auto& get() const noexcept
	{
		return (*this)[index];
	}

public:
	/// \brief Returns the number of elements that the array can hold.
	///
	/// \return Maximum number of element this array can hold.
	static constexpr Size capacity() noexcept { return maxCapacity; }
	static constexpr Size size() noexcept { return maxCapacity; }
};

template <class T, concepts::sameAs<T>... Ts>
constexpr auto makeArray(T&& t, Ts&&... ts) noexcept
{
	return bzd::Array<T, sizeof...(Ts) + 1u>{inPlace, bzd::forward<T>(t), bzd::forward<Ts>(ts)...};
}

} // namespace bzd

// Add support for structure bindings: `auto [a, b] = bzd::Array<int, 2>{inPlace, 1, 2.3};`
#include <tuple>

namespace std {
template <class T, ::bzd::Size maxCapacity>
struct tuple_size<::bzd::Array<T, maxCapacity>> : std::integral_constant<size_t, maxCapacity>
{
};
template <size_t index, class T, ::bzd::Size maxCapacity>
struct tuple_element<index, ::bzd::Array<T, maxCapacity>>
{
	using type = T;
};
} // namespace std
