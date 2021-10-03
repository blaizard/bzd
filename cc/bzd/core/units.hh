#pragma once

#include "cc/bzd/container/named_type.hh"
#include "cc/bzd/platform/types.hh"

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
} // namespace bzd::units
constexpr bzd::units::Meter operator""_m(unsigned long long length) noexcept
{
	return bzd::units::Meter{static_cast<bzd::units::Meter::UnderlyingType>(length)};
}
constexpr bzd::units::Kilometer operator""_km(unsigned long long length) noexcept
{
	return bzd::units::Kilometer{static_cast<bzd::units::Meter::UnderlyingType>(length)};
}
constexpr bzd::units::Hectometer operator""_hm(unsigned long long length) noexcept
{
	return bzd::units::Hectometer{static_cast<bzd::units::Meter::UnderlyingType>(length)};
}
constexpr bzd::units::Decameter operator""_dam(unsigned long long length) noexcept
{
	return bzd::units::Decameter{static_cast<bzd::units::Meter::UnderlyingType>(length)};
}
constexpr bzd::units::Decimeter operator""_dm(unsigned long long length) noexcept
{
	return bzd::units::Decimeter{static_cast<bzd::units::Meter::UnderlyingType>(length)};
}
constexpr bzd::units::Centimeter operator""_cm(unsigned long long length) noexcept
{
	return bzd::units::Centimeter{static_cast<bzd::units::Meter::UnderlyingType>(length)};
}
constexpr bzd::units::Millimeter operator""_mm(unsigned long long length) noexcept
{
	return bzd::units::Millimeter{static_cast<bzd::units::Meter::UnderlyingType>(length)};
}
constexpr bzd::units::Micrometer operator""_um(unsigned long long length) noexcept
{
	return bzd::units::Micrometer{static_cast<bzd::units::Meter::UnderlyingType>(length)};
}
constexpr bzd::units::Nanometer operator""_nm(unsigned long long length) noexcept
{
	return bzd::units::Nanometer{static_cast<bzd::units::Meter::UnderlyingType>(length)};
}
/// \}

/**
 * \brief Time units.
 */
namespace bzd::units {
using Second = bzd::NamedType<bzd::Int64Type, struct SecondTag, bzd::Arithmetic>;
using Day = bzd::MultipleOf<Second, bzd::Ratio<86400>, bzd::Arithmetic>;
using Hour = bzd::MultipleOf<Second, bzd::Ratio<3600>, bzd::Arithmetic>;
using Minute = bzd::MultipleOf<Second, bzd::Ratio<60>, bzd::Arithmetic>;
using Millisecond = bzd::MultipleOf<Second, bzd::ratio::Milli, bzd::Arithmetic>;
using Microsecond = bzd::MultipleOf<Second, bzd::ratio::Micro, bzd::Arithmetic>;
using Nanosecond = bzd::MultipleOf<Second, bzd::ratio::Nano, bzd::Arithmetic>;
} // namespace bzd::units
constexpr bzd::units::Second operator""_s(unsigned long long length) noexcept
{
	return bzd::units::Second{static_cast<bzd::units::Second::UnderlyingType>(length)};
}
constexpr bzd::units::Day operator""_day(unsigned long long length) noexcept
{
	return bzd::units::Day{static_cast<bzd::units::Second::UnderlyingType>(length)};
}
constexpr bzd::units::Hour operator""_hour(unsigned long long length) noexcept
{
	return bzd::units::Hour{static_cast<bzd::units::Second::UnderlyingType>(length)};
}
constexpr bzd::units::Minute operator""_min(unsigned long long length) noexcept
{
	return bzd::units::Minute{static_cast<bzd::units::Second::UnderlyingType>(length)};
}
constexpr bzd::units::Millisecond operator""_ms(unsigned long long length) noexcept
{
	return bzd::units::Millisecond{static_cast<bzd::units::Second::UnderlyingType>(length)};
}
constexpr bzd::units::Microsecond operator""_us(unsigned long long length) noexcept
{
	return bzd::units::Microsecond{static_cast<bzd::units::Second::UnderlyingType>(length)};
}
constexpr bzd::units::Nanosecond operator""_ns(unsigned long long length) noexcept
{
	return bzd::units::Nanosecond{static_cast<bzd::units::Second::UnderlyingType>(length)};
}
/// \}
