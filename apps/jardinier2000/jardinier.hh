#pragma once

#include "apps/jardinier2000/composition.hh"
#include "cc/bzd.hh"

namespace jardinier {

bzd::Async<> water(bzd::Timer& timer, bzd::Size wateringTimeS, bzd::UInt64 wakeUpPeriodS);

}
