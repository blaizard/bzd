#pragma once

#include "cc/bzd/container/impl/span.hh"
#include "cc/bzd/container/storage/non_owning.hh"
#include "cc/bzd/platform/types.hh"

namespace bzd {

/// \brief The class template span describes an object that can refer to a
/// contiguous sequence of objects with the first element of the sequence at
/// position zero.
///
/// \tparam T Element type, must be a complete type that is not an abstract class
/// type.
template <class T>
class Span : public impl::Span<T, impl::NonOwningStorage<T>>
{
private:
	using Parent = impl::Span<T, impl::NonOwningStorage<T>>;
	using ValueType = typename Parent::ValueType;
	using StorageType = typename Parent::StorageType;

public: // Constructors
	using impl::Span<T, impl::NonOwningStorage<T>>::Span;

	constexpr Span(ValueType* const data, const Size size) noexcept : Parent{StorageType{data, size}} {}

	template <bzd::Size N>
	constexpr Span(ValueType (&data)[N]) noexcept : Parent{StorageType{data, N}}
	{
	}
};

} // namespace bzd

// Implementation of Span specific impl::Span functions.
namespace bzd::impl {
template <class T, class Storage>
constexpr auto Span<T, Storage>::asSpan() const noexcept
{
	return bzd::Span<StorageValueType>{data(), size()};
}

template <class T, class Storage>
constexpr auto Span<T, Storage>::asSpan() noexcept
{
	return bzd::Span<StorageValueMutableType>{data(), size()};
}

template <class T, class Storage>
constexpr auto Span<T, Storage>::subSpan(const Size offset, const Size count) const noexcept
{
	bzd::assert::isTrue(offset <= size());
	const auto actualCount = (count == npos) ? (size() - offset) : count;
	bzd::assert::isTrue(offset + actualCount <= size());
	return bzd::Span<StorageValueType>{data() + offset, actualCount};
}

template <class T, class Storage>
constexpr auto Span<T, Storage>::subSpan(const Size offset, const Size count) noexcept
{
	bzd::assert::isTrue(offset <= size());
	const auto actualCount = (count == npos) ? (size() - offset) : count;
	bzd::assert::isTrue(offset + actualCount <= size());
	return bzd::Span<StorageValueMutableType>{data() + offset, actualCount};
}

template <class T, class Storage>
constexpr auto Span<T, Storage>::first(const Size count) const noexcept
{
	return subSpan(0, count);
}

template <class T, class Storage>
constexpr auto Span<T, Storage>::first(const Size count) noexcept
{
	return subSpan(0, count);
}

template <class T, class Storage>
constexpr auto Span<T, Storage>::last(const Size count) const noexcept
{
	return subSpan(size() - count, count);
}

template <class T, class Storage>
constexpr auto Span<T, Storage>::last(const Size count) noexcept
{
	return subSpan(size() - count, count);
}

template <class T, class Storage>
constexpr auto Span<T, Storage>::asBytes() const noexcept
{
	// This is ok, it complies with the type aliasing rules (because bzd::Byte === std::byte)
	return bzd::Span<const bzd::Byte>{reinterpret_cast<const bzd::Byte*>(data()), sizeBytes()};
}

template <class T, class Storage>
constexpr auto Span<T, Storage>::asBytesMutable() noexcept
{
	// This is ok, it complies with the type aliasing rules (because bzd::Byte === std::byte)
	return bzd::Span<bzd::Byte>{reinterpret_cast<bzd::Byte*>(data()), sizeBytes()};
}
} // namespace bzd::impl
