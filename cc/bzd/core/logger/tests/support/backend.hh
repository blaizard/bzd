#include "cc/bzd/container/array.hh"
#include "cc/bzd/core/logger/backend/logger.hh"
#include "cc/bzd/platform/types.hh"

namespace bzd::test {

template <bzd::Size SIZE>
class Logger : public bzd::OStream
{
public:
	Async<> write(const Span<const Byte> data) noexcept override
	{
		for (const auto b : data)
		{
			buffer_.at(write_++ % SIZE) = b;
		}
		co_return {};
	}

	/**
	 * Check that the next data are matching the one passed into argument.
	 */
	Result<> match(const Span<const Byte> data) noexcept
	{
		if (write_ - read_ > SIZE)
		{
			return bzd::error::make();
		}

		enum class Mode
		{
			normal,
			integer
		};
		Mode mode = Mode::normal;
		Size i = 0;
		for (; read_ < write_ && i < data.size(); ++read_)
		{
			const auto c = buffer_.at(read_ % SIZE);
			switch (mode)
			{
			case Mode::normal:
				{
					const auto expected = data.at(i);
					if (expected == bzd::Byte{'#'})
					{
						mode = Mode::integer;
						--read_;
					}
					else if (c == expected)
					{
						++i;
					}
					else
					{
						return bzd::error::make();
					}
				}
				break;
			case Mode::integer:
				if (c < bzd::Byte{'0'} || c > bzd::Byte{'9'})
				{
					mode = Mode::normal;
					--read_;
					++i;
				}
				break;
			}
		}

		if (i < data.size())
		{
			return bzd::error::make();
		}

		return nullresult;
	}

	/**
	 * Reset the data structure to its initial state.
	 */
	void reset()
	{
		read_ = 0;
		write_ = 0;
	}

private:
	bzd::Size read_{0};
	bzd::Size write_{0};
	bzd::Array<bzd::Byte, SIZE> buffer_;
};

} // namespace bzd::test
