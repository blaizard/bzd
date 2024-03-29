#pragma once

#include "cc/bzd/container/optional.hh"

/// Execute a lambda at a given inject point.
/// Usage:
/// ```
/// ExecPoint my_code{[&]() {
///     std::cout << "Hello" << std::endl;
/// }};
///
///	queue.push<bzd::test::InjectPoint0, decltype(my_code)>(element);
/// ```

namespace bzd::test {

template <class Callback, typename tag = void>
class ExecPoint
{
private:
	/// A singleton is need to ensure the definition of the template variable during
	/// compilation time, this prevents a linker issue.
	struct Wrapper
	{
		static auto& getInstance() noexcept
		{
			static Wrapper wrapper;
			return wrapper;
		}
		bzd::Optional<Callback> callback;
	};

public:
	constexpr ExecPoint() noexcept { Wrapper::getInstance().callback.value()(); }

	explicit ExecPoint(Callback&& callback) noexcept { Wrapper::getInstance().callback.emplace(bzd::move(callback)); }
};

} // namespace bzd::test
