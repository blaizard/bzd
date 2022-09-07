#pragma once

#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/enable_if.hh"
#include "cc/bzd/type_traits/is_const.hh"
#include "cc/bzd/type_traits/remove_const.hh"
#include "cc/bzd/utility/forward.hh"

namespace bzd::impl {
/// \brief Non-owning storage type.
template <class T>
class NonOwningStorage
{
public:
	using Self = NonOwningStorage<T>;
	using SelfNonConst = NonOwningStorage<bzd::typeTraits::RemoveConst<T>>;
	using IsDataConst = bzd::typeTraits::IsConst<T>;
	using ValueType = T;
	using ValueMutableType = T;

public: // Constructors
	// Default/copy/move constructor/assignment.
	constexpr NonOwningStorage() noexcept = default;
	constexpr NonOwningStorage(const Self&) noexcept = default;
	constexpr Self& operator=(const Self&) noexcept = default;
	constexpr NonOwningStorage(Self&&) noexcept = default;
	constexpr Self& operator=(Self&&) noexcept = default;

	explicit constexpr NonOwningStorage(T* const data, const bzd::Size size) noexcept : data_{data}, size_{size} {}

	// Ability to construct a const storage from a non-const
	template <class Q = IsDataConst, bzd::typeTraits::EnableIf<Q::value, void>* = nullptr>
	constexpr NonOwningStorage(const SelfNonConst& storage) noexcept : data_{storage.data_}, size_{storage.size_}
	{
	}

public: // Accessors
	constexpr ValueType* data() const noexcept { return data_; }
	constexpr ValueMutableType* dataMutable() noexcept { return data_; }
	constexpr bzd::Size size() const noexcept { return size_; }
	constexpr bzd::Size& sizeMutable() noexcept { return size_; }

public: // Modifier
	constexpr void removePrefix(const Size n) noexcept
	{
		data_ += n;
		size_ -= n;
	}

private:
	template <class U>
	friend class NonOwningStorage;

	T* data_{};
	bzd::Size size_{0};
};
} // namespace bzd::impl
