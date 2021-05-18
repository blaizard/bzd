#pragma once

#include "bzd/platform/types.h"
#include "bzd/type_traits/remove_const.h"
#include "bzd/utility/forward.h"

namespace bzd::impl {
/**
 * \brief Non-owning storage type.
 */
template <class T>
class NonOwningStorage
{
public:
	using Self = NonOwningStorage<T>;
	using SelfNonConst = NonOwningStorage<bzd::typeTraits::RemoveConst<T>>;
	using IsDataConst = bzd::typeTraits::IsConst<T>;
	using DataType = T;
	using DataMutableType = T;

public: // Constructors
	// Default/copy/move constructor/assignment.
	constexpr NonOwningStorage() noexcept = default;
	constexpr NonOwningStorage(const Self&) noexcept = default;
	constexpr Self& operator=(const Self&) noexcept = default;
	constexpr NonOwningStorage(Self&&) noexcept = default;
	constexpr Self& operator=(Self&&) noexcept = default;

	explicit constexpr NonOwningStorage(T* const data, const bzd::SizeType size) noexcept : data_{data}, size_{size} {}

	// Ability to construct a const storage from a non-const
	template <class Q = IsDataConst, bzd::typeTraits::EnableIf<Q::value, void>* = nullptr>
	constexpr NonOwningStorage(const NonOwningStorage<bzd::typeTraits::RemoveConst<T>>& storage) noexcept :
		data_{storage.data_}, size_{storage.size_}
	{
	}

public: // Accessors
	constexpr DataType* data() const noexcept { return data_; }
	constexpr DataMutableType*& dataMutable() noexcept { return data_; }
	constexpr bzd::SizeType size() const noexcept { return size_; }
	constexpr bzd::SizeType& sizeMutable() noexcept { return size_; }

private:
	template <class U>
	friend class NonOwningStorage;

	T* data_{};
	bzd::SizeType size_{0};
};
} // namespace bzd::impl
