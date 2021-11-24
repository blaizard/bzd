#include "cc/bzd/core/channel.hh"

#include <poll.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

namespace bzd::platform::std {
class In : public bzd::IStream
{
public: // Constructors
	constexpr In() noexcept
	{
		// Save the old context.
		::tcgetattr(0, &old_);
		old_.c_lflag |= ECHO;

		// Create the new one.
		current_ = old_;
		// Disable buffered i/o.
		current_.c_lflag &= ~ICANON;
		// Set no echo mode.
		current_.c_lflag &= ~ECHO;
		::tcsetattr(0, TCSANOW, &current_);
	}

	~In() { ::tcsetattr(0, TCSANOW, &old_); }

public: // API
	bzd::Async<bzd::Span<bzd::ByteType>> read(const bzd::Span<bzd::ByteType> data) noexcept override
	{
		if (data.size() == 0)
		{
			co_return bzd::error(ErrorType::failure, CSTR("Empty buffer passed to read(...)."));
		}
		while (true)
		{
			::pollfd fd{};
			fd.fd = STDIN_FILENO;
			fd.events = POLLIN;
			const auto ret = ::poll(&fd, 1, 0);
			if (ret < 0)
			{
				co_return bzd::error(ErrorType::failure, CSTR("Failed ::poll(...)."));
			}
			if (ret > 0 && (fd.revents & POLLIN) != 0)
			{
				const auto size = ::read(STDIN_FILENO, data.data(), data.size());
				if (size == -1)
				{
					co_return bzd::error(ErrorType::failure, CSTR("Failed ::read(...)."));
				}
				co_return data.subSpan(0, static_cast<bzd::SizeType>(size));
			}
			co_await bzd::async::yield();
		}
	}

private:
	termios old_{};
	termios current_{};
};
} // namespace bzd::platform::std
