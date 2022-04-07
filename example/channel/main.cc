#include "cc/bzd/container/result.hh"
#include "cc/bzd/container/string.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/core/channel.hh"

#include <iostream>

// This is a demo of what a channel implementation could look like

using ConstBufferView = bzd::Span<const bzd::UInt8Type>;
using BufferView = bzd::Span<bzd::UInt8Type>;

template <class T = BufferView>
class IOChannel
{
public:
	virtual bzd::Result<bzd::SizeType> write(const T& data) noexcept = 0;
	// virtual bzd::Result<bzd::SizeType> read(const T& data) noexcept = 0;
};

class I2CTransport
{
public:
	bzd::Result<bzd::SizeType> write(const int address, const ConstBufferView& data) noexcept
	{
		std::cout << "[address=" << address << "] ";
		for (auto c : data)
		{
			std::cout << c;
		}
		std::cout << std::endl;
		return data.size();
	}

	bzd::Result<bzd::SizeType> read(const int /*address*/, const BufferView& data) noexcept { return data.size(); }
};

template <class Data, class Transport>
class Adapter : public IOChannel<Data>
{
public:
	Adapter(Transport& transport) : transport_{transport} {}

protected:
	Transport& transport_;
};

template <class Data = ConstBufferView>
class I2CDevice : public Adapter<Data, I2CTransport>
{
public:
	I2CDevice(I2CTransport& transport, int address) : Adapter<Data, I2CTransport>{transport}, address_{address} {}

	bzd::Result<bzd::SizeType> write(const Data& data) noexcept override
	{
		this->transport_.write(address_, data);
		return data.size();
	}

private:
	int address_;
};

class Transport : public bzd::IOStream
{
public:
	bzd::Async<bzd::SizeType> write(const bzd::Span<const bzd::ByteType> data) noexcept override
	{
		if (data.size() < 2)
		{
			co_return bzd::error(bzd::ErrorType::failure, "Wrong size"_csv);
		}
		std::cout << "[id=" << static_cast<int>(data[0]) << "] [length=" << static_cast<int>(data[1]) << "] ";
		for (auto it = data.begin() + 2; it != data.end(); ++it)
		{
			std::cout << static_cast<char>(*it);
		}
		std::cout << std::endl;
		co_return data.size();
	}

	bzd::Async<bzd::Span<bzd::ByteType>> read(const bzd::Span<bzd::ByteType> data) noexcept override { co_return data; }
};

class Adapter2
{
public:
	Adapter2(bzd::IOStream& transport, const uint8_t id) : transport_{transport}, id_{id} {}

	struct Data
	{
		bzd::String<40> str;
	};

	template <typename T>
	bzd::Result<> serialize(const T&) const
	{
		return bzd::nullresult;
	}

	template <typename T>
	bzd::Result<> deserialize(const T&) const
	{
		return bzd::nullresult;
	}

	bool filter(const bzd::Span<const bzd::UInt8Type>&) { return true; }

private:
	bzd::IOStream& transport_;
	const uint8_t id_;
};

/**
 * <id> <length> <payload>
 */
template <>
bzd::Result<> Adapter2::serialize<Adapter2::Data>(const Data& data) const
{
	bzd::UInt8Type buffer[42] = {id_, static_cast<bzd::UInt8Type>(data.str.size())};
	bzd::algorithm::copy(data.str.begin(), data.str.end(), &buffer[2]);
	transport_.write(bzd::Span<const bzd::UInt8Type>{buffer, 2 + data.str.size()}.asBytes());
	return bzd::nullresult;
}
/*
int main()
{
	I2CTransport bus{};
	I2CDevice<> tempSensor{bus, 12};
	// bzd::String<40> str{"Hello!"};

	// tempSensor.write(str);

	Transport transport{};
	Adapter2 adapter{transport, 42};

	Adapter2::Data data;
	data.str = "Hello World!";
	adapter.serialize(data);

	return 0;
}
*/
