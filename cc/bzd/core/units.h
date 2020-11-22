#pragma once

#include "bzd/container/named_type.h"
#include "bzd/platform/types.h"

/**
 * \brief Length units.
 * \{
 */
namespace bzd::units {
using Meter = bzd::NamedType<bzd::Int32Type, struct MeterTag, bzd::Arithmetic>;
using Kilometer = bzd::MultipleOf<Meter, bzd::ratio::Kilo, bzd::Arithmetic>;
using Hectometer = bzd::MultipleOf<Meter, bzd::ratio::Hecto, bzd::Arithmetic>;
using Decameter = bzd::MultipleOf<Meter, bzd::ratio::Deca, bzd::Arithmetic>;
using Decimeter = bzd::MultipleOf<Meter, bzd::ratio::Deci, bzd::Arithmetic>;
using Centimeter = bzd::MultipleOf<Meter, bzd::ratio::Centi, bzd::Arithmetic>;
using Millimeter = bzd::MultipleOf<Meter, bzd::ratio::Milli, bzd::Arithmetic>;
using Micrometer = bzd::MultipleOf<Meter, bzd::ratio::Micro, bzd::Arithmetic>;
using Nanometer = bzd::MultipleOf<Meter, bzd::ratio::Nano, bzd::Arithmetic>;
using Picometer = bzd::MultipleOf<Meter, bzd::ratio::Pico, bzd::Arithmetic>;
} // namespace bzd::units
constexpr bzd::units::Meter operator""_m(unsigned long long length) noexcept
{
	return bzd::units::Meter{length};
}
constexpr bzd::units::Kilometer operator""_km(unsigned long long length) noexcept
{
	return bzd::units::Kilometer{length};
}
constexpr bzd::units::Hectometer operator""_hm(unsigned long long length) noexcept
{
	return bzd::units::Hectometer{length};
}
constexpr bzd::units::Decameter operator""_dam(unsigned long long length) noexcept
{
	return bzd::units::Decameter{length};
}
constexpr bzd::units::Decimeter operator""_dm(unsigned long long length) noexcept
{
	return bzd::units::Decimeter{length};
}
constexpr bzd::units::Centimeter operator""_cm(unsigned long long length) noexcept
{
	return bzd::units::Centimeter{length};
}
constexpr bzd::units::Millimeter operator""_mm(unsigned long long length) noexcept
{
	return bzd::units::Millimeter{length};
}
constexpr bzd::units::Micrometer operator""_um(unsigned long long length) noexcept
{
	return bzd::units::Micrometer{length};
}
constexpr bzd::units::Nanometer operator""_nm(unsigned long long length) noexcept
{
	return bzd::units::Nanometer{length};
}
constexpr bzd::units::Picometer operator""_pm(unsigned long long length) noexcept
{
	return bzd::units::Picometer{length};
}
/// \}

/**
 * \brief Time units.
 */
namespace bzd::units {
using Second = bzd::NamedType<bzd::Int32Type, struct SecondTag, bzd::Arithmetic>;
using Day = bzd::MultipleOf<Second, bzd::Ratio<86400>, bzd::Arithmetic>;
using Hour = bzd::MultipleOf<Second, bzd::Ratio<3600>, bzd::Arithmetic>;
using Minute = bzd::MultipleOf<Second, bzd::Ratio<60>, bzd::Arithmetic>;
using Millisecond = bzd::MultipleOf<Second, bzd::ratio::Milli, bzd::Arithmetic>;
using Microsecond = bzd::MultipleOf<Second, bzd::ratio::Micro, bzd::Arithmetic>;
using Nanosecond = bzd::MultipleOf<Second, bzd::ratio::Nano, bzd::Arithmetic>;
using Picosecond = bzd::MultipleOf<Second, bzd::ratio::Pico, bzd::Arithmetic>;
} // namespace bzd::units
constexpr bzd::units::Second operator""_s(unsigned long long length) noexcept
{
	return bzd::units::Second{length};
}
constexpr bzd::units::Day operator""_day(unsigned long long length) noexcept
{
	return bzd::units::Day{length};
}
constexpr bzd::units::Hour operator""_hour(unsigned long long length) noexcept
{
	return bzd::units::Hour{length};
}
constexpr bzd::units::Minute operator""_min(unsigned long long length) noexcept
{
	return bzd::units::Minute{length};
}
constexpr bzd::units::Millisecond operator""_ms(unsigned long long length) noexcept
{
	return bzd::units::Millisecond{length};
}
constexpr bzd::units::Microsecond operator""_us(unsigned long long length) noexcept
{
	return bzd::units::Microsecond{length};
}
constexpr bzd::units::Nanosecond operator""_ns(unsigned long long length) noexcept
{
	return bzd::units::Nanosecond{length};
}
constexpr bzd::units::Picosecond operator""_ps(unsigned long long length) noexcept
{
	return bzd::units::Picosecond{length};
}
/// \}
