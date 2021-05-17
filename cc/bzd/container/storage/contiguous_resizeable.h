#pragma once

#include "bzd/platform/types.h"

namespace bzd::impl {
/**
 * \brief Non-owning storage type.
 */
template <class T, bzd::SizeType N>
class ContiguousResizeableStorage
{
public: // Constructors
	constexpr ContiguousResizeableStorage() noexcept = default;

	template <class... Args>
	constexpr explicit ContiguousResizeableStorage(Args&&... args) noexcept : data_{bzd::forward<Args>(args)...}, size_{sizeof...(Args)} {}

public: // Accessors
	constexpr const T* data() const noexcept { return data_; }
	constexpr T* dataMutable() noexcept { return data_; }
	constexpr bzd::SizeType size() const noexcept { return size_; }
	constexpr bzd::SizeType& sizeMutable() noexcept { return size_; }

private:
	T data_[N]{};
	bzd::SizeType size_{0};
};
}
