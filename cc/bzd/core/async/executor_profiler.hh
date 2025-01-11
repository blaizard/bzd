#pragma once

#include "cc/bzd/platform/types.hh"

namespace bzd::async::profiler {

using UIdType = bzd::UInt16;

struct NewCore
{
	UIdType uid;
};

struct DeleteCore
{
	UIdType uid;
};

struct ExecutableScheduled
{
};

struct ExecutableUnscheduled
{
};

struct ExecutableCanceled
{
};

} // namespace bzd::async::profiler
