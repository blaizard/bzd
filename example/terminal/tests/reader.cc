#include "example/terminal/reader.hh"

#include "cc/bzd/test/test.hh"
#include "cc/components/generic/stream/stub/stub.hh"

template <bzd::SizeType N>
class MockIn : public bzd::IStream
{
public:
	void add(const bzd::StringView data)
	{
		for (const auto c : data)
		{
			buffer_.pushBack(static_cast<bzd::ByteType>(c));
		}
	}

	bzd::Async<bzd::Span<bzd::ByteType>> read(const bzd::Span<bzd::ByteType> data) noexcept override
	{
		if (buffer_.empty())
		{
			co_return bzd::Span<bzd::ByteType>{};
		}
		bzd::SizeType index{0};
		for (const auto b : buffer_.asSpanForReading())
		{
			if (index >= data.size())
			{
				break;
			}
			data[index++] = b;
			buffer_.consume(1);
		}
		co_return data.subSpan(0, index);
	}

private:
	bzd::RingBuffer<bzd::ByteType, N> buffer_{};
};

TEST_ASYNC(Reader, readUntil)
{
	bzd::platform::generic::stream::Stub out{};
	MockIn<16> mock{};
	bzd::Reader<16> reader{mock, out};

	mock.add("Hello:you");
	const auto result = co_await reader.readUntil(bzd::ByteType{':'});
	EXPECT_TRUE(result);
	// EXPECT_EQ(result.value(), "Hello"_sv);
}
