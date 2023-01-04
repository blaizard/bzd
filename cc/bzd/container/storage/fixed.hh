#pragma once

#include "cc/bzd/algorithm/copy_n.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/utility/distance.hh"
#include "cc/bzd/utility/forward.hh"
#include "cc/bzd/utility/in_place.hh"
#include "cc/bzd/utility/min.hh"
#include "cc/bzd/type_traits/conditional.hh"
#include "cc/bzd/container/reference_wrapper.hh"

namespace bzd::impl {
/// \brief Fixed storage type.
template <class T, bzd::Size capacity>
class FixedStorage
{
private:
	using TNoRef = typeTraits::RemoveReference<T>;

public:
	using Self = FixedStorage<T, capacity>;
	using ValueMutableType = T;
	using ValueType = const T;
	using StorageValueMutableType = typeTraits::Conditional<typeTraits::isReference<T>, bzd::ReferenceWrapper<TNoRef>, T>;
	using StorageValueType = const StorageValueMutableType;

public: // Constructors
	// Default/copy/move constructor/assignment.
	constexpr FixedStorage() noexcept = default;
	constexpr FixedStorage(const Self&) noexcept = default;
	constexpr Self& operator=(const Self&) noexcept = default;
	constexpr FixedStorage(Self&&) noexcept = default;
	constexpr Self& operator=(Self&&) noexcept = default;

	template <class... Args>
	constexpr explicit FixedStorage(InPlace, Args&&... args) noexcept requires (!concepts::reference<T>) : data_{bzd::forward<Args>(args)...}
	{
	}

	template <class... Args>
	constexpr explicit FixedStorage(InPlace, Args&&... args) noexcept requires (concepts::reference<T>) : data_{args...}
	{
	}

	constexpr FixedStorage(const TNoRef* begin, const TNoRef* end) noexcept
	{
		const auto n = bzd::min(capacity, static_cast<Size>(end - begin));
		bzd::algorithm::copyN(begin, n, data_);
	}

public: // Accessors
	constexpr StorageValueType* data() const noexcept { return data_; }
	constexpr StorageValueMutableType* dataMutable() noexcept { return data_; }
	constexpr bzd::Size size() const noexcept { return capacity; }
	static constexpr auto& storageToValue(auto& data) noexcept { return data; }

private:
	StorageValueMutableType data_[(capacity == 0) ? 1 : capacity]{};
};
} // namespace bzd::impl
