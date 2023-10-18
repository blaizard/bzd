#pragma once

#include "cc/bzd/algorithm/lower_bound.hh"
#include "cc/bzd/algorithm/move_backward.hh"
#include "cc/bzd/algorithm/sort.hh"
#include "cc/bzd/algorithm/upper_bound.hh"
#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/tuple.hh"
#include "cc/bzd/container/vector.hh"
#include "cc/bzd/core/assert/minimal.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/utility/comparison/less.hh"

#include <initializer_list>

namespace bzd::impl {
/// \brief Flat map implementation.
template <class K, class V, class Compare>
class Map
{
public:
	struct Element
	{
		K first;
		V second;

		constexpr Bool operator<(const Element& other) const { return first < other.first; }
	};
	using Iterator = typename bzd::interface::Vector<Element>::Iterator;

protected:
	constexpr explicit Map(bzd::interface::Vector<Element>& data) noexcept : data_(data) {}

public: // Iterators
	[[nodiscard]] constexpr auto begin() noexcept { return data_.begin(); }
	[[nodiscard]] constexpr auto begin() const noexcept { return data_.begin(); }
	[[nodiscard]] constexpr auto end() noexcept { return data_.end(); }
	[[nodiscard]] constexpr auto end() const noexcept { return data_.end(); }

public:
	/// Equal range in the map.
	[[nodiscard]] constexpr auto equalRange(const K& key) const noexcept { return bzd::makeTuple(lowerBound(key), upperBound(key)); }

	/// Lower bound in the map.
	[[nodiscard]] constexpr Iterator lowerBound(const K& key) const noexcept
	{
		return algorithm::lowerBound(data_.begin(), data_.end(), key, [this](const Element& elt, const K& value) {
			return compare_(elt.first, value);
		});
	}

	/// Upper bound in the map.
	[[nodiscard]] constexpr Iterator upperBound(const K& key) const noexcept
	{
		return algorithm::upperBound(data_.begin(), data_.end(), key, [this](const K& value, const Element& elt) {
			return compare_(value, elt.first);
		});
	}

	/// Search for a specific element in the map.
	[[nodiscard]] constexpr Iterator find(const K& key) const noexcept
	{
		const auto it = lowerBound(key);
		if (it != data_.end())
		{
			if (!compare_(key, it->first))
			{
				return it;
			}
		}
		return data_.end();
	}

	[[nodiscard]] constexpr V& operator[](const K& key) const
	{
		auto it = find(key);
		bzd::assert::isTrue(it != data_.end(), "Key does not exists");
		return it->second;
	}

	[[nodiscard]] constexpr bool contains(const K& key) const noexcept { return (find(key) != data_.end()); }

	/// Whether or not the map contains elements.
	[[nodiscard]] constexpr bool empty() const noexcept { return data_.empty(); }

	/// Get the number of elements in the map.
	[[nodiscard]] constexpr Size size() const noexcept { return data_.size(); }

	/// \brief Returns the maximum number of elements the map can hold.
	///
	/// \return Maximum number of element this map can hold.
	[[nodiscard]] constexpr Size capacity() const noexcept { return data_.capacity(); }

	/// Insert a new element or replace the existing one
	template <class U>
	constexpr void insert(const K& key, U&& value)
	{
		const auto it = lowerBound(key);
		if (it == data_.end())
		{
			data_.pushBack(Element{key, bzd::forward<U>(value)});
			return;
		}
		else if (compare_(key, it->first))
		{
			bzd::assert::isTrue(this->size() < capacity(), "Out of bound");
			++data_.storage_.sizeMutable();
			bzd::algorithm::moveBackward(it, data_.end() - 1, data_.end());
		}

		it->first = key;
		it->second = bzd::forward<U>(value);
	}

protected:
	bzd::interface::Vector<Element>& data_;
	Compare compare_{};
};
} // namespace bzd::impl

namespace bzd::interface {
template <class K, class V, class Compare = bzd::Less<K>>
using Map = impl::Map<K, V, Compare>;
}

namespace bzd {

template <class K, class V, Size N, class Compare = bzd::Less<K>>
class Map : public interface::Map<K, V, Compare>
{
private:
	using typename interface::Map<K, V, Compare>::Element;

public:
	constexpr Map() : interface::Map<K, V, Compare>{data_} {}
	constexpr Map(std::initializer_list<Element> list) : interface::Map<K, V, Compare>{data_}, data_{list}
	{
		bzd::algorithm::sort(data_.begin(), data_.end());
		// Ensure there is no duplicated keys
		if (data_.size() > 1)
		{
			auto it = data_.begin();
			for (const Element* previous = &(*it++); it != data_.end(); ++it)
			{
				// Ensure these 2 keys are different.
				bzd::assert::isTrue(this->compare_(previous->first, it->first) || this->compare_(it->first, previous->first),
									"Duplicated keys");
				previous = &(*it);
			}
		}
	}

protected:
	bzd::Vector<Element, N> data_;
}; // namespace bzd
} // namespace bzd
