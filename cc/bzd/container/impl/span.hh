#pragma once

#include "cc/bzd/algorithm/equal.hh"
#include "cc/bzd/algorithm/find.hh"
#include "cc/bzd/algorithm/lexicographical_compare.hh"
#include "cc/bzd/container/iterator/contiguous.hh"
#include "cc/bzd/core/assert/minimal.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/enable_if.hh"
#include "cc/bzd/type_traits/is_const.hh"
#include "cc/bzd/type_traits/remove_const.hh"
#include "cc/bzd/utility/distance.hh"
#include "cc/bzd/utility/forward.hh"
#include "cc/bzd/utility/move.hh"

#include <new> // operator new for "placement new"

namespace bzd {
template <class T>
class Span;
}

namespace bzd::impl {
/// \brief The class template span describes an object that can refer to a
/// contiguous sequence of objects with the first element of the sequence at
/// position zero.
///
/// \tparam T Element type, must be a complete type that is not an abstract class
/// type.
template <class T, class Storage>
class Span
{
protected:
	using ValueType = T;
	using StorageType = Storage;
	using Self = Span<ValueType, StorageType>;
	using IsDataConst = bzd::typeTraits::IsConst<ValueType>;
	using StorageValueType = typename StorageType::ValueType;
	using StorageValueMutableType = typename StorageType::ValueMutableType;

public:
	using ConstIterator = bzd::iterator::Contiguous<StorageValueType>;
	using Iterator = bzd::iterator::Contiguous<StorageValueMutableType>;

public: // Constructor/assignment
	// Default/copy/move constructor/assignment.
	constexpr Span() noexcept = default;
	constexpr Span(const Self&) noexcept = default;
	constexpr Self& operator=(const Self&) noexcept = default;
	constexpr Span(Self&&) noexcept = default;
	constexpr Self& operator=(Self&&) noexcept = default;

	constexpr Span(const Storage& storage) noexcept : storage_{storage} {}

	// Forward copy constraint to the storage type.
	template <class U, class V>
	constexpr Span(const Span<U, V>& span) noexcept : storage_(span.storage_)
	{
	}

	// Forward move constraint to the storage type.
	template <class U, class V>
	constexpr Span(Span<U, V>&& span) noexcept : storage_(bzd::move(span.storage_))
	{
	}

public: // Iterators
	[[nodiscard]] constexpr auto begin() noexcept { return Iterator{data()}; }
	[[nodiscard]] constexpr auto begin() const noexcept { return ConstIterator{data()}; }
	[[nodiscard]] constexpr auto end() noexcept { return Iterator{&data()[size()]}; }
	[[nodiscard]] constexpr auto end() const noexcept { return ConstIterator{&data()[size()]}; }

public: // Size
	[[nodiscard]] constexpr Size size() const noexcept { return storage_.size(); }
	[[nodiscard]] constexpr Size sizeBytes() const noexcept { return size() * sizeof(ValueType); }
	[[nodiscard]] constexpr Bool empty() const noexcept { return (size() == 0); }

public: // Comparison operators
	[[nodiscard]] constexpr Bool operator==(const Self& other) const noexcept
	{
		if (size() == other.size())
		{
			return bzd::algorithm::equal(begin(), end(), other.begin());
		}
		return false;
	}

	[[nodiscard]] constexpr Bool operator!=(const Self& other) const noexcept { return !(*this == other); }

	[[nodiscard]] constexpr Bool operator<(const Self& other) const noexcept
	{
		return bzd::algorithm::lexicographicalCompare(begin(), end(), other.begin(), other.end());
	}

	[[nodiscard]] constexpr Bool operator>(const Self& other) const noexcept { return (other < *this); }

	[[nodiscard]] constexpr Bool operator<=(const Self& other) const noexcept { return !(*this > other); }

	[[nodiscard]] constexpr Bool operator>=(const Self& other) const noexcept { return !(*this < other); }

public: // Accessors
	[[nodiscard]] constexpr auto& operator[](const Size index) noexcept { return at(index); }
	[[nodiscard]] constexpr auto& operator[](const Size index) const noexcept { return at(index); }
	[[nodiscard]] constexpr auto& at(const Size index) noexcept { return data()[index]; }
	[[nodiscard]] constexpr auto& at(const Size index) const noexcept { return data()[index]; }
	[[nodiscard]] constexpr auto& front() const noexcept { return at(0); }
	[[nodiscard]] constexpr auto& front() noexcept { return at(0); }
	[[nodiscard]] constexpr auto& back() const noexcept { return at(size() - 1); }
	[[nodiscard]] constexpr auto& back() noexcept { return at(size() - 1); }
	[[nodiscard]] constexpr auto data() const noexcept { return storage_.data(); }
	[[nodiscard]] constexpr auto data() noexcept { return storage_.dataMutable(); }
	template <Size N>
	[[nodiscard]] constexpr auto& get() const noexcept
	{
		return at(N);
	}
	template <Size N>
	[[nodiscard]] constexpr auto& get() noexcept
	{
		return at(N);
	}

public: // Emplace
	template <class... Args>
	constexpr void emplace(bzd::iterator::Contiguous<ValueType> pos, Args&&... args) noexcept
	{
		pos->~ValueType();
		::new (&(*pos)) ValueType{bzd::forward<Args>(args)...};
	}

public: // Find
	[[nodiscard]] constexpr Size find(const ValueType& item, const Size start = 0) const noexcept
	{
		if (start < size())
		{
			const auto it = bzd::algorithm::find(begin() + start, end(), item);
			if (it != end())
			{
				return bzd::distance(begin(), it);
			}
		}
		return npos;
	}

	template <class... Args>
	[[nodiscard]] constexpr Bool contains(Args&&... args) const noexcept
	{
		return find(bzd::forward<Args>(args)...) != npos;
	}

public: // Subviews. Their definition is in bzd::Span
	[[nodiscard]] constexpr auto asSpan() const noexcept;
	[[nodiscard]] constexpr auto asSpan() noexcept;
	[[nodiscard]] constexpr auto subSpan(const Size offset = 0, const Size count = npos) const noexcept;
	[[nodiscard]] constexpr auto subSpan(const Size offset = 0, const Size count = npos) noexcept;
	[[nodiscard]] constexpr auto first(const Size count) const noexcept;
	[[nodiscard]] constexpr auto first(const Size count) noexcept;
	[[nodiscard]] constexpr auto last(const Size count) const noexcept;
	[[nodiscard]] constexpr auto last(const Size count) noexcept;

public: // Convert to bytes
	[[nodiscard]] constexpr auto asBytes() const noexcept;
	[[nodiscard]] constexpr auto asBytesMutable() noexcept;

protected:
	template <class U, class V>
	friend class Span;

	template <class K, class V, class Compare>
	friend class Map;

	StorageType storage_{};
};
} // namespace bzd::impl
