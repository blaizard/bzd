#pragma once

#include "bzd/core/channel.h"

namespace bzd::platform {
void reset(const FctPtrType fct, void* context);

void panic();
bzd::OChannel& getOut();
} // namespace bzd::platform
