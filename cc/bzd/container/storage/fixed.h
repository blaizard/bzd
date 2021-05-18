#pragma once

#include "bzd/platform/types.h"
#include "bzd/utility/forward.h"

namespace bzd::impl {
/**
 * \brief Fixed storage type.
 */
template <class T, bzd::SizeType N>
class FixedStorage
{
public:
	using Self = FixedStorage<T, N>;

public: // Constructors

	// Default/copy/move constructor/assignment.
	constexpr FixedStorage() noexcept = default;
	constexpr FixedStorage(const Self&) noexcept = default;
	constexpr Self& operator=(const Self&) noexcept = default;
	constexpr FixedStorage(Self&&) noexcept = default;
	constexpr Self& operator=(Self&&) noexcept = default;

	template <class... Args>
	constexpr explicit FixedStorage(Args&&... args) noexcept : data_{bzd::forward<Args>(args)...} {}

public: // Accessors
	constexpr const T* data() const noexcept { return data_; }
	constexpr T* dataMutable() noexcept { return data_; }
	constexpr bzd::SizeType size() const noexcept { return size_; }
	constexpr bzd::SizeType sizeMutable() noexcept { return size_; }

private:
	T data_[N]{};
	bzd::SizeType size_{N};
};
}
