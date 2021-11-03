#include "cc/bzd.hh"

namespace Example {

bzd::Async<bool> run()
{
	bzd::Array<bzd::ByteType, 128> buffer{};
	[[maybe_unused]] const auto result = co_await bzd::platform::in().read(buffer.asSpan());

	co_return true;
}

} // namespace Example
