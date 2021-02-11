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
		tcgetattr(0, &old_);			  /* grab old terminal i/o settings */
		current_ = old_;				  /* make new settings same as old settings */
		current_.c_lflag &= ~ICANON;	  /* disable buffered i/o */
		current_.c_lflag &= ~ECHO;		  /* set no echo mode */
		tcsetattr(0, TCSANOW, &current_); /* use these new terminal i/o settings now */
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

bzd::Result<bzd::SizeType> Terminal::read(const bzd::Span<bzd::ByteType>& data)
{
	bzd::ignore = impl::Terminal::getInstance();
	struct pollfd fd
	{
	};
	fd.fd = STDIN_FILENO;
	fd.events = POLLIN;
	const auto ret = ::poll(&fd, 1, 0);
	if (ret > 0 && ((fd.revents & POLLIN) != 0))
	{
		const auto size = ::read(STDIN_FILENO, data.data(), data.size());
		return size;
	}
	return bzd::makeError();
}
