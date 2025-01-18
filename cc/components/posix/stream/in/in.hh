#pragma once

#include "cc/bzd/core/channel.hh"
#include "cc/components/posix/stream/in/interface.hh"

#include <iostream>
#include <poll.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

namespace bzd::components::posix {
template <class Context>
class In : public bzd::IStream
{
public: // Constructors
	constexpr In(Context& context) noexcept : context_{context}
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

		// Stream properly initialized.
		init_ = true;
	}

	~In()
	{
		if (init_)
		{
			::tcsetattr(in_.native(), TCSANOW, &old_);
		}
	}

protected: // API
	bzd::Generator<bzd::Span<const bzd::Byte>> readerImpl(bzd::Span<bzd::Byte> data) noexcept final
	{
		if (!init_)
		{
			co_yield bzd::error::Failure("No terminal."_csv);
		}
		while (true)
		{
			co_yield (co_await context_.config.proactor.read(in_, bzd::move(data)));
		}
	}

private:
	Context& context_;
	bzd::Bool init_{false};
	termios old_{};
	termios current_{};
	FileDescriptor in_{STDIN_FILENO};
};
} // namespace bzd::components::posix
