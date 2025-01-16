#pragma once

#include "cc/bzd/test/types/ichannel.hh"

namespace bzd::test {

template <class T, bzd::Size capacity, IChannelMode mode = IChannelMode::none>
class IChannelGenerator : public bzd::test::IChannel<T, capacity, mode>
{
private:
	using Parent = IChannel<T, capacity, mode>;

public:
	IChannelGenerator() = default;
	constexpr explicit IChannelGenerator(const T start) noexcept : Parent{}, current_{start} {}

private:
	bzd::Async<bzd::Span<const T>> read(bzd::Span<T> data) noexcept override
	{
		fill();
		return Parent::read(data);
	}

	constexpr void fill() noexcept
	{
		while (!this->buffer_.full())
		{
			this->buffer_.pushBack(current_);
			++current_;
		}
	}

private:
	T current_{};
};

} // namespace bzd::test
