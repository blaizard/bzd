#pragma once

#include "cc/bzd.hh"

namespace jardinier {

bzd::Async<bool> water(bzd::SizeType wateringTimeS, bzd::UInt64Type wakeUpPeriodS);

}
