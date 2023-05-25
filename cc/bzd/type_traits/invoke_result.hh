#pragma once

#include <type_traits>

namespace bzd::typeTraits {
template <class F, class... Args>
using InvokeResult = typename std::invoke_result<F, Args...>::type;
}
