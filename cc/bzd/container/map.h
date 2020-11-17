#pragma once

#include "bzd/container/optional.h"
#include "bzd/container/vector.h"
#include "bzd/core/assert/minimal.h"
#include "bzd/platform/types.h"

namespace bzd::impl {
/**
 * \brief Flat map implementation.
 */
template <class K, class V>
class Map
{
public:
	struct Element
	{
		K first;
		V second;
	};
	using Iterator = typename bzd::interface::Vector<Element>::Iterator;

public:
	constexpr explicit Map(bzd::interface::Vector<Element>& data) noexcept : data_(data) {}

	/**
	 * Search for a specific element in the map.
	 */
	constexpr bzd::Optional<Iterator> find(const K& key) const noexcept
	{
		for (auto it = data_.begin(); it != data_.end(); ++it)
		{
			if (it->first == key)
			{
				return it;
			}
		}
		return bzd::nullopt;
	}

	constexpr V& operator[](const K& key) const
	{
		auto result = find(key);
		bzd::assert::isTrue(result, "Key does not exists");
		return (*result)->second;
	}

	constexpr bool contains(const K& key) const noexcept { return find(key); }

	/**
	 * Insert a new element or replace the existing one
	 */
	constexpr void insert(const K& key, V&& value)
	{
		auto result = find(key);
		if (result)
		{
			insert(*result, bzd::forward<V>(value));
		}
		else
		{
			data_.pushBack({key, bzd::forward<V>(value)});
		}
	}

	constexpr void insert(const Iterator& it, V&& value) { it->second = bzd::forward<V>(value); }

protected:
	bzd::interface::Vector<Element>& data_;
};
} // namespace bzd::impl

namespace bzd::interface {
template <class K, class V>
using Map = impl::Map<K, V>;
}

namespace bzd {

template <class K, class V, SizeType N>
class Map : public interface::Map<K, V>
{
private:
	using typename interface::Map<K, V>::Element;

public:
	constexpr Map() : interface::Map<K, V>(data_) {}

protected:
	bzd::Vector<Element, N> data_;
};
} // namespace bzd
