#pragma once

#include "cc/bzd/container/array.hh"
#include "cc/bzd/math/ceil.hh"
#include "cc/bzd/platform/atomic.hh"
#include "cc/bzd/utility/bit/count_lsb_one.hh"
#include "cc/bzd/utility/bit/count_lsb_zero.hh"
#include "cc/bzd/utility/min.hh"

#include <iostream>

namespace bzd::threadsafe {

/// Thread safe implementation of a bitset.
///
/// \tparam n The number of bits this bitset holds
template <Size n>
class Bitset
{
private:
	using Self = Bitset<n>;
	using UnitType = UInt32;
	static constexpr Size bitsPerUnit = sizeof(UnitType) * 8u;
	static constexpr Size unitCount = bzd::ceil(n * 1.0f / bitsPerUnit);

public:
	/// Default constructor. Constructs a bitset with all bits set to zero.
	Bitset() = default;

	/// Create a bitset from a string filled with '0' and '1's.
	template <Size dataN>
	requires(dataN - 1u == n) constexpr Bitset(const char (&data)[dataN]) noexcept
	{
		for (Size index = 0, unitIndex = 0; index < n; ++unitIndex)
		{
			UnitType value = 0;
			for (Size indexWithinUnit = 0; index < n && indexWithinUnit < bitsPerUnit; ++indexWithinUnit, ++index)
			{
				switch (data[index])
				{
				case '0':
					break;
				case '1':
					value = setWithinUnit(value, indexWithinUnit);
					break;
				default:
					bzd::assert::unreachable();
				}
			}
			data_[unitIndex].store(value);
		}
	}

public:
	constexpr Bool operator==(const Self& other) const noexcept
	{
		for (Size unitIndex = 0; unitIndex < unitCount; ++unitIndex)
		{
			if (data_[unitIndex] != other.data_[unitIndex])
			{
				return false;
			}
		}
		return true;
	}

public:
	/// Returns the number of bits that the bitset holds.
	constexpr Size size() const noexcept { return n; }

	/// Returns the number of consecutive 0 bits, starting from the left (lower index).
	///
	/// \return The number of consecutive 0 bits.
	constexpr Size countLZero() const noexcept
	{
		Size total = 0u;
		for (const auto& unit : data_)
		{
			const auto count = bzd::countLSBZero(unit.load());
			total += count;
			if (count < bitsPerUnit)
			{
				break;
			}
		}
		return bzd::min(total, n);
	}

	/// Returns the number of consecutive 1 bits, starting from the left (lower index).
	///
	/// \return The number of consecutive 1 bits.
	constexpr Size countLOne() const noexcept
	{
		Size total = 0u;
		for (const auto& unit : data_)
		{
			const auto count = bzd::countLSBOne(unit.load());
			total += count;
			if (count < bitsPerUnit)
			{
				break;
			}
		}
		return bzd::min(total, n);
	}

	constexpr Bool compareSet(const Size index, const MemoryOrder order = MemoryOrder::sequentiallyConsistent) noexcept
	{
		assert::isTrue(index < n);
		const auto unitIndex = index / bitsPerUnit;
		const auto indexWithinUnit = index % bitsPerUnit;

		auto expected = data_[unitIndex].load();
		do
		{
			expected = clearWithinUnit(expected, indexWithinUnit);
			const auto desired = setWithinUnit(expected, indexWithinUnit);
			if (data_[unitIndex].compareExchange(expected, desired, order))
			{
				return true;
			}
		} while (!testWithinUnit(expected, indexWithinUnit));

		return false;
	}

private:
	constexpr UnitType setWithinUnit(const UnitType unit, const Size indexWithinUnit) const noexcept
	{
		return unit | (1u << indexWithinUnit);
	}

	constexpr UnitType clearWithinUnit(const UnitType unit, const Size indexWithinUnit) const noexcept
	{
		return unit & ~(1u << indexWithinUnit);
	}

	constexpr Bool testWithinUnit(const UnitType unit, const Size indexWithinUnit) const noexcept
	{
		return (unit & (1u << indexWithinUnit)) != 0u;
	}

private:
	Array<Atomic<UnitType>, unitCount> data_{};
};

template <Size dataN>
Bitset(const char (&data)[dataN]) -> Bitset<dataN - 1u>;

} // namespace bzd::threadsafe
