#pragma once

#include "bzd/container/iostream.h"
#include "bzd/container/string_view.h"

namespace bzd {
void panic();
bzd::OStream& getOut();
} // namespace bzd
