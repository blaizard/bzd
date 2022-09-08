#include "cc/libs/reader/stream_reader.hh"

#include "cc/bzd/test/types/istream.hh"
#include "cc/bzd/test/types/istream_reader.hh"
#include "cc/bzd/test/test.hh"

TEST_ASYNC(StreamReader, Basic)
{
    bzd::test::IStream<64u> in{};
    bzd::StreamReader<64u> reader{in};

    in << "hello";
    {
        const auto output = co_await !bzd::test::read(reader);
        EXPECT_STREQ(output.data(), "hello");
    }
    {
        const auto output = co_await !bzd::test::read(reader);
        EXPECT_STREQ(output.data(), "");
    }

    co_return {};
}
