#pragma once

#include "cc/bzd/algorithm/sort.hh"
#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/tuple.hh"
#include "cc/bzd/container/vector.hh"
#include "cc/bzd/core/assert/minimal.hh"
#include "cc/bzd/platform/types.hh"

#include <initializer_list>

namespace bzd::impl {
/// \brief Flat map implementation.
template <class K, class V>
class Map
{
public:
	struct Element
	{
		K first;
		V second;

		constexpr BoolType operator<(const Element& other) const { return first < other.first; }
	};
	using Iterator = typename bzd::interface::Vector<Element>::Iterator;

public:
	constexpr explicit Map(bzd::interface::Vector<Element>& data) noexcept : data_(data) {}

	/// Search for a specific element in the map.
	[[nodiscard]] constexpr bzd::Optional<Iterator> find(const K& key) const noexcept
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

	[[nodiscard]] constexpr V& operator[](const K& key) const
	{
		auto result = find(key);
		bzd::assert::isTrue(result.hasValue(), "Key does not exists");
		return result.valueMutable()->second;
	}

	[[nodiscard]] constexpr bool contains(const K& key) const noexcept { return find(key).hasValue(); }

	/// Whether or not the map contains elements.
	[[nodiscard]] constexpr bool empty() const noexcept { return data_.empty(); }

	/// Get the number of elements in the map.
	[[nodiscard]] constexpr SizeType size() const noexcept { return data_.size(); }

	/// Insert a new element or replace the existing one
	template <class U>
	constexpr void insert(const K& key, U&& value)
	{
		auto result = find(key);
		if (result)
		{
			insert(result.value(), bzd::forward<U>(value));
		}
		else
		{
			data_.pushBack({key, bzd::forward<U>(value)});
		}
	}

	template <class U>
	constexpr void insert(const Iterator& it, U&& value)
	{
		it->second = bzd::forward<U>(value);
	}

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
	constexpr Map() : interface::Map<K, V>{data_} {}
	constexpr Map(std::initializer_list<Element> list) : interface::Map<K, V>{data_}, data_{list}
	{
		bzd::algorithm::sort(data_.begin(), data_.end());
		// Ensure there is no duplicated keys
		if (data_.size() > 1)
		{
			auto it = data_.begin();
			for (const Element* previous = &(*it++); it != data_.end(); ++it)
			{
				bzd::assert::isTrue(previous->first != it->first, "Duplicated keys");
				previous = &(*it);
			}
		}
	}

protected:
	bzd::Vector<Element, N> data_;
};
} // namespace bzd
