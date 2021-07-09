#pragma once

#include "cc/bzd/container/function_view.h"
#include "cc/bzd/container/result.h"
#include "cc/bzd/container/vector.h"

using Void = void;
using Callable = bzd::FunctionView<void(void)>;
using Integer = bzd::Int32Type;
using Float = float;
template <class V, class E>
using Result = bzd::Result<V, E>;
template <class T>
using List = bzd::interface::Vector<T>;
