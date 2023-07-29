#pragma once

#include "cc/bzd/algorithm/equal.hh"
#include "cc/bzd/algorithm/find.hh"
#include "cc/bzd/algorithm/lexicographical_compare.hh"
#include "cc/bzd/algorithm/rsearch.hh"
#include "cc/bzd/algorithm/search.hh"
#include "cc/bzd/core/assert/minimal.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/enable_if.hh"
#include "cc/bzd/type_traits/is_const.hh"
#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/type_traits/remove_const.hh"
#include "cc/bzd/utility/forward.hh"
#include "cc/bzd/utility/iterators/contiguous.hh"
#include "cc/bzd/utility/iterators/distance.hh"
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
	using StorageType = Storage;
	using ValueType = typename StorageType::ValueType;
	using ValueMutableType = typename StorageType::ValueMutableType;
	using StorageValueType = typename StorageType::StorageValueType;
	using StorageValueMutableType = typename StorageType::StorageValueMutableType;
	using Self = Span<T, StorageType>;
	using IsDataConst = bzd::typeTraits::IsConst<ValueType>;

	template <class V, class S>
	struct DefaultPolicies : public bzd::iterator::impl::DefaultPolicies<V>
	{
		using StorageValueType = S;
		static constexpr auto& at(auto* data, const Size n) noexcept { return StorageType::storageToValue(data[n]); }
	};

public:
	using ConstIterator = bzd::iterator::Contiguous<ValueType, void, DefaultPolicies<ValueType, StorageValueType>>;
	using Iterator = bzd::iterator::Contiguous<ValueMutableType, void, DefaultPolicies<ValueMutableType, StorageValueMutableType>>;

public: // Constructor/assignment
	// Default/copy/move constructor/assignment.
	Span() = default;
	Span(const Self&) = default;
	Self& operator=(const Self&) = default;
	Span(Self&&) = default;
	Self& operator=(Self&&) = default;
	~Span() = default;

	constexpr Span(const Storage& storage) noexcept : storage_{storage} {}

	// Forward copy constraint to the storage type.
	template <concepts::convertible<T> U, class V>
	constexpr Span(const Span<U, V>& span) noexcept : storage_(span.storage_)
	{
	}

	// Forward move constraint to the storage type.
	template <concepts::convertible<T> U, class V>
	constexpr Span(Span<U, V>&& span) noexcept : storage_(bzd::move(span.storage_))
	{
	}

	// Construct from a pair of iterator.
	template <concepts::contiguousIterator U, concepts::sentinelFor<U> V>
	requires(concepts::convertible<typeTraits::IteratorValue<U>, ValueMutableType>)
	constexpr Span(U it, V end) noexcept : storage_(&(*it), bzd::distance(it, end))
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

	/// Check if this is a subview of another span.
	template <class U, class V>
	[[nodiscard]] constexpr Bool isSubSpan(const Span<U, V>& other) const noexcept
	{
		return (size() > 0) && (data() >= other.data() && &(data()[size()]) <= &(other.data()[other.size()]));
	}

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
	template <Size index>
	[[nodiscard]] constexpr auto& get() const noexcept
	{
		return at(index);
	}
	template <Size index>
	[[nodiscard]] constexpr auto& get() noexcept
	{
		return at(index);
	}

public: // Modifiers
	template <class... Args>
	constexpr void emplace(ConstIterator position, Args&&... args) noexcept
	{
		position->~ValueType();
		::new (&(*position)) ValueType{bzd::forward<Args>(args)...};
	}

public: // Find
	/// Searches the span for the first occurrence of the sequence specified by its arguments.
	[[nodiscard]] constexpr Size find(const Self& sequence, const Size start = 0) const noexcept
	{
		if (start < size())
		{
			const auto it = bzd::algorithm::search(begin() + start, end(), sequence.begin(), sequence.end());
			if (it != end())
			{
				return bzd::distance(begin(), it);
			}
		}
		return npos;
	}

	/// Searches the span for the first occurrence of the value specified by its arguments.
	[[nodiscard]] constexpr Size find(const ValueType& value, const Size start = 0) const noexcept
	{
		if (start < size())
		{
			const auto it = bzd::algorithm::find(begin() + start, end(), value);
			if (it != end())
			{
				return bzd::distance(begin(), it);
			}
		}
		return npos;
	}

	/// Searches the span for the last occurrence of the sequence specified by its arguments.
	[[nodiscard]] constexpr Size rfind(const Self& sequence, const Size start = 0) const noexcept
	{
		if (start < size())
		{
			const auto it = bzd::algorithm::rsearch(begin() + start, end(), sequence.begin(), sequence.end());
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
