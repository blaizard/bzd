#pragma once

#include "bzd/platform/types.h"
#include "bzd/utility/forward.h"

namespace bzd::impl {
/**
 * \brief Non-owning storage type.
 */
template <class T>
class NonOwningStorage
{
public: // Constructors
	constexpr NonOwningStorage() noexcept = default;
	explicit constexpr NonOwningStorage(T* const data, const bzd::SizeType size) noexcept : data_{data}, size_{size} {}

public: // Accessors
	constexpr T* data() const noexcept { return data_; }
	constexpr T*& dataMutable() noexcept { return data_; }
	constexpr bzd::SizeType size() const noexcept { return size_; }
	constexpr bzd::SizeType& sizeMutable() noexcept { return size_; }

private:
	T* data_{};
	bzd::SizeType size_{0};
};
}
