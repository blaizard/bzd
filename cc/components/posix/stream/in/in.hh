#include "cc/bzd/core/channel.hh"
#include "cc/components/posix/stream/in/interface.hh"

#include <iostream>
#include <poll.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

namespace bzd::platform::posix {
template <class Proactor>
class In : public bzd::IStream
{
public: // Constructors
	constexpr In(Proactor& proactor) noexcept : proactor_{proactor}
	{
		// Check if the terminal is available from this process
		if (::tcgetpgrp(in_.native()) != ::getpgrp())
		{
			return;
		}

		// Save the old context.
		::tcgetattr(in_.native(), &old_);

		// Create the new one.
		current_ = old_;
		// Disable buffered i/o.
		current_.c_lflag &= ~ICANON;
		// Set no echo mode.
		current_.c_lflag &= ~ECHO;
		::tcsetattr(in_.native(), TCSANOW, &current_);

		// Stream properly initalized.
		init_ = true;
	}

	~In()
	{
		if (init_)
		{
			::tcsetattr(in_.native(), TCSANOW, &old_);
		}
	}

public: // API
	bzd::Async<bzd::Span<const bzd::Byte>> read(bzd::Span<bzd::Byte>&& data) noexcept final
	{
		if (!init_)
		{
			co_return bzd::error::Failure("No terminal."_csv);
		}
		co_return (co_await proactor_.read(in_, bzd::move(data)));
	}

private:
	Proactor& proactor_;
	bzd::Bool init_{false};
	termios old_{};
	termios current_{};
	FileDescriptor in_{STDIN_FILENO};
};
} // namespace bzd::platform::posix
