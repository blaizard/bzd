#include "cc/bzd/core/channel.hh"

#include <iostream>
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
		// Check if the terminal is available from this process
		if (::tcgetpgrp(STDIN_FILENO) != ::getpgrp())
		{
			return;
		}

		// Save the old context.
		::tcgetattr(STDIN_FILENO, &old_);
		old_.c_lflag |= ECHO;

		// Create the new one.
		current_ = old_;
		// Disable buffered i/o.
		current_.c_lflag &= ~ICANON;
		// Set no echo mode.
		current_.c_lflag &= ~ECHO;
		::tcsetattr(STDIN_FILENO, TCSANOW, &current_);

		// Stream properly initalized.
		init_ = true;
	}

	~In()
	{
		if (init_)
		{
			::tcsetattr(STDIN_FILENO, TCSANOW, &old_);
		}
	}

public: // API
	bzd::Async<bzd::Span<bzd::ByteType>> read(const bzd::Span<bzd::ByteType> data) noexcept override
	{
		if (!init_)
		{
			co_return bzd::error(ErrorType::failure, "No terminal."_csv);
		}
		if (data.size() == 0)
		{
			co_return bzd::error(ErrorType::failure, "Empty buffer passed to read(...)."_csv);
		}
		while (true)
		{
			::pollfd fd{};
			fd.fd = STDIN_FILENO;
			fd.events = POLLIN;
			const auto ret = ::poll(&fd, 1, 0);
			if (ret < 0)
			{
				co_return bzd::error(ErrorType::failure, "Failed ::poll(...) with errno {}."_csv, errno);
			}
			if (ret > 0 && (fd.revents & POLLIN) != 0)
			{
				const auto size = ::read(STDIN_FILENO, data.data(), data.size());
				if (size < 0)
				{
					co_return bzd::error(ErrorType::failure, "Failed ::read(...) with errno {}."_csv, errno);
				}
				co_return data.subSpan(0, static_cast<bzd::SizeType>(size));
			}
			co_await bzd::async::yield();
		}
	}

private:
	bzd::BoolType init_{false};
	termios old_{};
	termios current_{};
};
} // namespace bzd::platform::std
