#pragma once

#include "bzd/core/channel.h"

namespace bzd::platform {
void panic();
bzd::OChannel& getOut();
} // namespace bzd::platform
