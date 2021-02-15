#include "example/terminal.h"

#include "bzd/utility/singleton.h"

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

	bzd::Promise<bzd::SizeType>::ReturnType promise(bzd::interface::Promise&, bzd::AnyReference& arg) const noexcept
	{
		::pollfd fd{};
		fd.fd = STDIN_FILENO;
		fd.events = POLLIN;
		const auto ret = ::poll(&fd, 1, 0);
		if (ret < 0)
		{
			return bzd::makeError();
		}
		if (ret > 0 && (fd.revents & POLLIN) != 0)
		{
			const auto& data = arg.cast<const bzd::Span<bzd::ByteType>>();
			const auto size = ::read(STDIN_FILENO, data.data(), data.size());
			return size;
		}
		return bzd::nullopt;
	}

	~Terminal() { tcsetattr(0, TCSANOW, &old_); }

private:
	struct termios old_, current_;
};
} // namespace impl

bzd::Result<bzd::SizeType> Terminal::write(const bzd::Span<const bzd::ByteType>& /*data*/)
{
	return 0;
}

bzd::Promise<bzd::SizeType> Terminal::read(const bzd::Span<bzd::ByteType>& data)
{
	auto& terminal = impl::Terminal::getInstance();
	return bzd::Promise<bzd::SizeType>{bzd::Promise<bzd::SizeType>::FunctionViewType{terminal, &impl::Terminal::promise}, data};
}
