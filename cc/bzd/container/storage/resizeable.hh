#pragma once

#include "cc/bzd/platform/types.hh"
#include "cc/bzd/utility/forward.hh"

namespace bzd::impl {
/// \brief Resizeable storage type.
template <class T, bzd::SizeType N>
class ResizeableStorage
{
public:
	using Self = ResizeableStorage<T, N>;
	using ValueType = const T;
	using ValueMutableType = T;

public: // Constructors
	// Default/copy/move constructor/assignment.
	constexpr ResizeableStorage() noexcept = default;
	constexpr ResizeableStorage(const Self&) noexcept = default;
	constexpr Self& operator=(const Self&) noexcept = default;
	constexpr ResizeableStorage(Self&&) noexcept = default;
	constexpr Self& operator=(Self&&) noexcept = default;

	template <class... Args>
	constexpr explicit ResizeableStorage(Args&&... args) noexcept : data_{bzd::forward<Args>(args)...}, size_{sizeof...(Args)}
	{
	}

public: // Accessors
	constexpr ValueType* data() const noexcept { return data_; }
	constexpr ValueMutableType* dataMutable() noexcept { return data_; }
	constexpr bzd::SizeType size() const noexcept { return size_; }
	constexpr bzd::SizeType& sizeMutable() noexcept { return size_; }

private:
	T data_[(N == 0) ? 1 : N]{};
	bzd::SizeType size_{0};
};
} // namespace bzd::impl
