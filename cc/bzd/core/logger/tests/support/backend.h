#include "cc/bzd/container/array.h"
#include "cc/bzd/core/logger/backend/logger.h"
#include "cc/bzd/platform/types.h"

#include <iostream>
namespace bzd::test {

template <bzd::SizeType SIZE>
class Logger : public bzd::backend::Logger
{
public:
	Async<SizeType> write(const Span<const ByteType> data) noexcept override
	{
		for (const auto b : data)
		{
			buffer_.at(write_++ % SIZE) = b;
			std::cout << static_cast<char>(b) << std::flush;
		}
		co_return 0;
	}

	/**
	 * Check that the next data are matching the one passed into argument.
	 */
	Result<> match(const Span<const ByteType> data) noexcept
	{
		if (write_ - read_ > SIZE)
		{
			return makeError();
		}

		SizeType i = 0;
		for (; read_ < write_ && i < data.size(); ++read_, ++i)
		{
			if (buffer_.at(read_ % SIZE) != data.at(i))
			{
				return makeError();
			}
		}

		if (i < data.size())
		{
			return makeError();
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
	bzd::SizeType read_{0};
	bzd::SizeType write_{0};
	bzd::Array<bzd::ByteType, SIZE> buffer_;
};

} // namespace bzd::test
