#pragma once

#include "cc/bzd.hh"

namespace jardinier {

bzd::Async<bool> water(bzd::Size wateringTimeS, bzd::UInt64 wakeUpPeriodS);

}
