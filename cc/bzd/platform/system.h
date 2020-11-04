#pragma once

#include "bzd/core/channel.h"

namespace bzd::platform {
void contextSwitch(void** stack1, void* stack2) noexcept;
void* contextTask() noexcept;
void reset(const FctPtrType fct, void* context);

void panic();
bzd::OChannel& getOut();
} // namespace bzd::platform
