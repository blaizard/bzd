#include "cc/components/generic/network/tcp/client.hh"

#include "cc/bzd/test/test.hh"
#include "tools/bdl/generators/cc/adapter/context.hh"

namespace bzd::components::generic::network {

template <class Read, class Write>
struct Config
{
	Read read;
	Write write;
};

TEST_ASYNC(Client, Simple)
{
	auto context = bzd::generator::makeContext(
		Config{.read = [](bzd::Span<Byte>&&) { return "Hello"_sv.asBytes(); }, .write = [](const bzd::Span<const Byte>) {}});

	tcp::Client client{context};

	auto stream = co_await !client.connect("", 12);

	bzd::Array<char, 100u> data;
	const auto span = co_await !stream.read(data.asBytesMutable());

	EXPECT_EQ_RANGE(span, "Hello"_sv.asBytes());

	co_return {};
}

} // namespace bzd::components::generic::network
