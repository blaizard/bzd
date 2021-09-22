#pragma once

#include "cc/bzd/container/impl/span.hh"
#include "cc/bzd/container/storage/non_owning.hh"
#include "cc/bzd/platform/types.hh"

namespace bzd {

/**
 * \brief The class template span describes an object that can refer to a
 * contiguous sequence of objects with the first element of the sequence at
 * position zero.
 *
 * \tparam T Element type, must be a complete type that is not an abstract class
 * type.
 */
template <class T>
class Span : public impl::Span<T, impl::NonOwningStorage<T>>
{
private:
	using Parent = impl::Span<T, impl::NonOwningStorage<T>>;
	using DataType = typename Parent::DataType;
	using StorageType = typename Parent::StorageType;

public: // Constructors
	using impl::Span<T, impl::NonOwningStorage<T>>::Span;

	constexpr Span(DataType* const data, const SizeType size) noexcept : Parent{StorageType{data, size}} {}

	template <bzd::SizeType N>
	constexpr Span(DataType (&data)[N]) noexcept : Parent{StorageType{data, N}}
	{
	}
};

} // namespace bzd

// Implementation of Span specific impl::Span functions.
namespace bzd::impl {
template <class T, class Storage>
constexpr auto Span<T, Storage>::subSpan(const SizeType offset, const SizeType count) const noexcept
{
	bzd::assert::isTrue(offset < size());
	const auto actualCount = (count == npos) ? (size() - offset) : count;
	bzd::assert::isTrue(offset + actualCount <= size());
	return bzd::Span<StorageDataType>{data() + offset, actualCount};
}

template <class T, class Storage>
constexpr auto Span<T, Storage>::subSpan(const SizeType offset, const SizeType count) noexcept
{
	bzd::assert::isTrue(offset < size());
	const auto actualCount = (count == npos) ? (size() - offset) : count;
	bzd::assert::isTrue(offset + actualCount <= size());
	return bzd::Span<StorageDataMutableType>{data() + offset, actualCount};
}

template <class T, class Storage>
constexpr auto Span<T, Storage>::first(const SizeType count) const noexcept
{
	return subSpan(0, count);
}

template <class T, class Storage>
constexpr auto Span<T, Storage>::first(const SizeType count) noexcept
{
	return subSpan(0, count);
}

template <class T, class Storage>
constexpr auto Span<T, Storage>::last(const SizeType count) const noexcept
{
	return subSpan(size() - count, count);
}

template <class T, class Storage>
constexpr auto Span<T, Storage>::last(const SizeType count) noexcept
{
	return subSpan(size() - count, count);
}

template <class T, class Storage>
constexpr auto Span<T, Storage>::asBytes() const noexcept
{
	return bzd::Span<const bzd::ByteType>{reinterpret_cast<const bzd::ByteType*>(data()), sizeBytes()};
}

template <class T, class Storage>
constexpr auto Span<T, Storage>::asWritableBytes() noexcept
{
	return bzd::Span<bzd::ByteType>{reinterpret_cast<bzd::ByteType*>(data()), sizeBytes()};
}
} // namespace bzd::impl
