#include "example/terminal.hh"

#include "cc/bzd.hh"
/*
#include <poll.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

namespace impl {
class Terminal : public bzd::Singleton<Terminal>
{
public:
	Terminal()
	{
		tcgetattr(0, &old_);
		old_.c_lflag |= ECHO;
		current_ = old_;
		// Disable buffered i/o
		current_.c_lflag &= ~ICANON;
		// Set no echo mode
		current_.c_lflag &= ~ECHO;
		tcsetattr(0, TCSANOW, &current_);
	}

	bzd::Promise<bzd::Size>::ReturnType promise(bzd::interface::Promise&, bzd::AnyReference& arg) const noexcept
	{
		::pollfd fd{};
		fd.fd = STDIN_FILENO;
		fd.events = POLLIN;
		const auto ret = ::poll(&fd, 1, 0);
		if (ret < 0)
		{
			return bzd::error();
		}
		if (ret > 0 && (fd.revents & POLLIN) != 0)
		{
			const auto& data = arg.cast<const bzd::Span<bzd::Byte>>();
			bzd::assert::isTrue(data.size(), "Empty buffer supplied.");
			const auto size = ::read(STDIN_FILENO, data.data(), data.size());
			return static_cast<unsigned long>(size);
		}
		return bzd::nullopt;
	}

	~Terminal() { tcsetattr(0, TCSANOW, &old_); }

private:
	struct termios old_, current_;
};
} // namespace impl

bzd::Promise<bzd::Size> Terminal::write(const bzd::Span<const bzd::Byte>& data) noexcept
{
	std::cout.write(reinterpret_cast<const char*>(data.data()), data.size());
	std::cout.flush();
	return bzd::Promise<bzd::Size>{data.size()};
}

bzd::Promise<bzd::Size> Terminal::read(const bzd::Span<bzd::Byte>& data) noexcept
{
	auto& terminal = impl::Terminal::getInstance();
	return bzd::Promise<bzd::Size>{bzd::Promise<bzd::Size>::FunctionViewType{terminal, &impl::Terminal::promise}, data};
}
*/
