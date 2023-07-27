#pragma once

/// Forward declaration of Async/Generator.

namespace bzd {

class Error;

template <class V = void, class E = bzd::Error>
class Async;

template <class V = void, class E = bzd::Error>
class Generator;

} // namespace bzd
