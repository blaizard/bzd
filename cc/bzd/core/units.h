#pragma once

#include "bzd/container/named_type.h"
#include "bzd/platform/types.h"

namespace bzd::units {
/**
 * \brief Length units.
 * \{
 */
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
/// \}

/**
 * \brief Time units.
 */
using Second = bzd::NamedType<bzd::Int32Type, struct SecondTag, bzd::Arithmetic>;
using Day = bzd::MultipleOf<Second, bzd::Ratio<86400>, bzd::Arithmetic>;
using Hour = bzd::MultipleOf<Second, bzd::Ratio<3600>, bzd::Arithmetic>;
using Minute = bzd::MultipleOf<Second, bzd::Ratio<60>, bzd::Arithmetic>;
using Millisecond = bzd::MultipleOf<Second, bzd::ratio::Milli, bzd::Arithmetic>;
using Microsecond = bzd::MultipleOf<Second, bzd::ratio::Micro, bzd::Arithmetic>;
using Nanosecond = bzd::MultipleOf<Second, bzd::ratio::Nano, bzd::Arithmetic>;
using Picosecond = bzd::MultipleOf<Second, bzd::ratio::Pico, bzd::Arithmetic>;
/// \}
}
