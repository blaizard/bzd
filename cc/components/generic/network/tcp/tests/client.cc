#include "cc/bdl/generator/impl/adapter/context.hh"
#include "cc/bzd/test/test.hh"
#include "cc/bzd/utility/ranges/views_async/join.hh"
#include "cc/components/generic/network/tcp/tests/mock.hh"

namespace bzd::components::generic::network {

TEST_ASYNC(Client, Simple)
{
	tcp::MockClientWithString client{"Hello"_sv.asBytes(), "World"_sv.asBytes()};

	auto stream = co_await !client.connect("", 12);

	bzd::Array<char, 100u> data;

	auto reader = stream.reader(data.asBytesMutable());
	EXPECT_EQ_RANGE(reader | bzd::ranges::join(), "Hello"_sv.asBytes());

	co_await !stream.write("World"_sv.asBytes());

	co_return {};
}

} // namespace bzd::components::generic::network
