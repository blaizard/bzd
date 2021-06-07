#pragma once

#include "cc/bzd/container/result.h"
#include "cc/bzd/container/function_view.h"

using Callable = bzd::FunctionView<void(void)>;
using Integer = bzd::Int32Type;
template <class V, class E>
using Result = bzd::Result<V, E>;
