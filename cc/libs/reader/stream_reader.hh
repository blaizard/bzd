#pragma once

#include "cc/libs/reader/channel_reader.hh"

namespace bzd {

template <Size capacity>
class StreamReader : public ChannelReader<bzd::Byte, capacity>
{
public: // Constructors.
	using ChannelReader<bzd::Byte, capacity>::ChannelReader;
};

} // namespace bzd
