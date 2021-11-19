#pragma once

#include "cc/bzd/container/iterator/contiguous.hh"
#include "cc/bzd/core/assert/minimal.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/enable_if.hh"
#include "cc/bzd/type_traits/is_const.hh"
#include "cc/bzd/type_traits/remove_const.hh"
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
	using DataType = T;
	using StorageType = Storage;
	using Self = Span<DataType, StorageType>;
	using IsDataConst = bzd::typeTraits::IsConst<DataType>;
	using StorageDataType = typename StorageType::DataType;
	using StorageDataMutableType = typename StorageType::DataMutableType;

public:
	using ConstIterator = bzd::iterator::Contiguous<StorageDataType>;
	using Iterator = bzd::iterator::Contiguous<StorageDataMutableType>;

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
	constexpr auto begin() noexcept { return Iterator{data()}; }
	constexpr auto begin() const noexcept { return ConstIterator{data()}; }
	constexpr auto end() noexcept { return Iterator{&data()[size()]}; }
	constexpr auto end() const noexcept { return ConstIterator{&data()[size()]}; }

public: // Size
	constexpr SizeType size() const noexcept { return storage_.size(); }
	constexpr SizeType sizeBytes() const noexcept { return size() * sizeof(DataType); }
	constexpr bool empty() const noexcept { return (size() == 0); }

public: // Operators
	constexpr bool operator==(const Self& rhs) const noexcept
	{
		if (size() != rhs.size())
		{
			return false;
		}

		for (bzd::SizeType index = 0; index < size(); ++index)
		{
			if (at(index) != rhs.at(index))
			{
				return false;
			}
		}

		return true;
	}

	constexpr bool operator!=(const Self& rhs) const noexcept { return !(*this == rhs); }

public: // Accessors
	constexpr auto& operator[](const SizeType index) noexcept { return at(index); }
	constexpr auto& operator[](const SizeType index) const noexcept { return at(index); }
	constexpr auto& at(const SizeType index) noexcept { return data()[index]; }
	constexpr auto& at(const SizeType index) const noexcept { return data()[index]; }
	constexpr auto& front() const noexcept { return at(0); }
	constexpr auto& front() noexcept { return at(0); }
	constexpr auto& back() const noexcept { return at(size() - 1); }
	constexpr auto& back() noexcept { return at(size() - 1); }
	constexpr auto data() const noexcept { return storage_.data(); }
	constexpr auto data() noexcept { return storage_.dataMutable(); }
	template <SizeType N>
	constexpr auto& get() const noexcept
	{
		return at(N);
	}
	template <SizeType N>
	constexpr auto& get() noexcept
	{
		return at(N);
	}

public: // Emplace
	template <class... Args>
	constexpr void emplace(bzd::iterator::Contiguous<DataType> pos, Args&&... args) noexcept
	{
		pos->~DataType();
		::new (&(*pos)) DataType{bzd::forward<Args>(args)...};
	}

public: // Find
	constexpr SizeType find(const DataType& item, const SizeType start = 0) const noexcept
	{
		for (SizeType i = start; i < size(); ++i)
		{
			if (at(i) == item)
			{
				return i;
			}
		}
		return npos;
	}

public: // Subviews. Their definition is in bzd::Span
	constexpr auto asSpan() const noexcept;
	constexpr auto asSpan() noexcept;
	constexpr auto subSpan(const SizeType offset = 0, const SizeType count = npos) const noexcept;
	constexpr auto subSpan(const SizeType offset = 0, const SizeType count = npos) noexcept;
	constexpr auto first(const SizeType count) const noexcept;
	constexpr auto first(const SizeType count) noexcept;
	constexpr auto last(const SizeType count) const noexcept;
	constexpr auto last(const SizeType count) noexcept;

public: // Convert to bytes
	constexpr auto asBytes() const noexcept;
	constexpr auto asBytesMutable() noexcept;

protected:
	template <class U, class V>
	friend class Span;

	StorageType storage_{};
};
} // namespace bzd::impl
