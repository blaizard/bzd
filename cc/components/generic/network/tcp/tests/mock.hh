#pragma once

#include "cc/bzd/test/test.hh"
#include "cc/components/generic/network/tcp/client.hh"

namespace bzd::components::generic::network::tcp {

class MockClientWithString : public Client<void>
{
public:
	constexpr explicit MockClientWithString(bzd::Span<const bzd::Byte> read) noexcept : read_{read} {}
	constexpr MockClientWithString(bzd::Span<const bzd::Byte> read, bzd::Span<const bzd::Byte> write) noexcept : read_{read}, write_{write}
	{
	}

	bzd::Span<const bzd::Byte> read(bzd::Span<Byte>&& data, Metadata& metadata) noexcept override
	{
		const auto outputSize = bzd::min(data.size(), static_cast<Size>(read_.size() - metadata.index));
		const auto output = read_.subSpan(metadata.index, outputSize);
		metadata.index += output.size();
		return output;
	}

	void write(const bzd::Span<const Byte> data, Metadata& metadata) noexcept override
	{
		if (write_)
		{
			const auto writeSize = bzd::min(data.size(), static_cast<Size>(write_->size() - metadata.index));
			const auto write = write_->subSpan(metadata.index, writeSize);
			EXPECT_EQ_RANGE(write, data);
			metadata.index += write.size();
		}
	}

private:
	const bzd::Span<const bzd::Byte> read_;
	bzd::Optional<bzd::Span<const bzd::Byte>> write_{};
};

} // namespace bzd::components::generic::network::tcp
