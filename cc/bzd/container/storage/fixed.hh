#pragma once

#include "cc/bzd/platform/types.hh"
#include "cc/bzd/utility/forward.hh"

namespace bzd::impl {
/// \brief Fixed storage type.
template <class T, bzd::SizeType N>
class FixedStorage
{
public:
	using Self = FixedStorage<T, N>;
	using DataType = const T;
	using DataMutableType = T;

public: // Constructors
	// Default/copy/move constructor/assignment.
	constexpr FixedStorage() noexcept = default;
	constexpr FixedStorage(const Self&) noexcept = default;
	constexpr Self& operator=(const Self&) noexcept = default;
	constexpr FixedStorage(Self&&) noexcept = default;
	constexpr Self& operator=(Self&&) noexcept = default;

	template <class... Args>
	constexpr explicit FixedStorage(Args&&... args) noexcept : data_{bzd::forward<Args>(args)...}
	{
	}

public: // Accessors
	constexpr DataType* data() const noexcept { return data_; }
	constexpr DataMutableType* dataMutable() noexcept { return data_; }
	constexpr bzd::SizeType size() const noexcept { return N; }

private:
	T data_[(N == 0) ? 1 : N]{};
};
} // namespace bzd::impl
