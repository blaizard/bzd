#include <iostream>

#include "bzd/core/channel.h"
#include "bzd/container/expected.h"
#include "bzd/container/string_view.h"
#include "bzd/container/string.h"

// This is a demo of what a channel implementation could look like

class Transport : public bzd::IOChannel
{
public:
	bzd::Expected<bzd::SizeType> write(const bzd::Span<const bzd::UInt8Type>& data) noexcept override
	{
		if (data.size() < 2) {
			return bzd::makeUnexpected(0);
		}
		std::cout << "[id=" << static_cast<int>(data[0]) << "] [length=" << static_cast<int>(data[1]) << "] ";
		for (auto it = data.begin() + 2; it != data.end(); ++it) {
			std::cout << static_cast<char>(*it); 
		}
		std::cout << std::endl;
		return data.size();
	}

	bzd::Expected<bzd::SizeType> read(bzd::Span<bzd::UInt8Type>& data) noexcept override
	{
		return data.size();
	}
};

class Adapter
{
public:
	Adapter(bzd::IOChannel& transport, const uint8_t id) : transport_{transport}, id_{id}
	{
	}

	struct Data
	{
		bzd::String<40> str;
	};

	template <typename T>
	bzd::Expected<void> serialize(const T&) const {
		return {};
	}

	template <typename T>
	bzd::Expected<void> deserialize(const T&) const {
		return {};
	}

	bool filter(const bzd::Span<const bzd::UInt8Type>&)
	{
		return true;
	}

private:
	bzd::IOChannel& transport_;
	const uint8_t id_;
};

/**
 * <id> <length> <payload>
 */
template <>
bzd::Expected<void> Adapter::serialize<Adapter::Data>(const Data& data) const
{
	bzd::UInt8Type buffer[42] = {id_, static_cast<bzd::UInt8Type>(data.str.size())};
	bzd::algorithm::copy(data.str.begin(), data.str.end(), &buffer[2]);
	transport_.write(bzd::Span<const bzd::UInt8Type>{buffer, 2 + data.str.size()});
	return {};
}

class Channel
{
public:
	Channel(bzd::OChannel& transport, Adapter& adapter) : transport_{transport}, adapter_{adapter} {}

	template <typename T>
	void write(const T& data)
	{
	}

private:
	bzd::OChannel& transport_;
	Adapter& adapter_;
};

int main()
{
	Transport transport{};
	Adapter adapter{transport, 42};

	Adapter::Data data;
	data.str = "Hello World!";
	adapter.serialize(data);

	return 0;
}
