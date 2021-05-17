#pragma once

#include "bzd/container/iterator/contiguous.h"
#include "bzd/core/assert/minimal.h"
#include "bzd/platform/types.h"
#include "bzd/type_traits/enable_if.h"
#include "bzd/type_traits/is_const.h"
#include "bzd/type_traits/remove_const.h"

namespace bzd {
	template <class T>
	class Span;
}

namespace bzd::impl {
/**
 * \brief The class template span describes an object that can refer to a
 * contiguous sequence of objects with the first element of the sequence at
 * position zero.
 *
 * \tparam T Element type, must be a complete type that is not an abstract class
 * type.
 */
template <class T, class Storage>
class Span
{
protected:
	using DataType = T;
	using StorageType = Storage;
	using Self = Span<DataType, StorageType>;
	using IsDataConst = bzd::typeTraits::IsConst<DataType>;

public:
	using Iterator = bzd::iterator::Contiguous<DataType>;
	using ConstIterator = bzd::iterator::Contiguous<DataType>;

	static constexpr const SizeType npos = static_cast<SizeType>(-1);

public: // Constructor/assignment

	// Default/copy/move constructor/assignment.
	constexpr Span() noexcept = default;
	constexpr Span(const Self&) noexcept = default;
	constexpr Self& operator=(const Self&) noexcept = default;
	constexpr Span(Self&&) noexcept = default;
	constexpr Self& operator=(Self&&) noexcept = default;

	constexpr Span(const Storage& storage) noexcept : storage_{storage} {}

	template <class Q = IsDataConst, bzd::typeTraits::EnableIf<Q::value, void>* = nullptr>
	constexpr Span(const Span<bzd::typeTraits::RemoveConst<DataType>, StorageType>& span) noexcept : storage_(storage_)
	{
	}

public: // Iterators

	constexpr auto begin() noexcept { return bzd::iterator::Contiguous(data(), 0); }
	constexpr auto begin() const noexcept { return bzd::iterator::Contiguous(data(), 0); }
	constexpr auto end() noexcept { return bzd::iterator::Contiguous(data(), size()); }
	constexpr auto end() const noexcept { return bzd::iterator::Contiguous(data(), size()); }

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

	constexpr bzd::Span<DataType> subSpan(const SizeType offset = 0, const SizeType count = npos) const noexcept;
	constexpr bzd::Span<DataType> first(const SizeType count) const noexcept;
	constexpr bzd::Span<DataType> last(const SizeType count) const noexcept;

public: // Convert to bytes

	constexpr bzd::Span<const bzd::ByteType> asBytes() const noexcept;
	constexpr bzd::Span<bzd::ByteType> asWritableBytes() const noexcept;

protected:
	StorageType storage_{};
};
} // namespace bzd
