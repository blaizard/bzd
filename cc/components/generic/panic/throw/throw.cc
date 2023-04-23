#include "cc/bzd/platform/panic.hh"

#include <iostream>

void bzd::platform::panic(const bzd::FunctionRef<void(bzd::OStream&)>) { throw 42; }
