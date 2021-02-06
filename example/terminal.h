#include <termios.h>
#include <stdio.h>
#include <poll.h>
#include <iostream>
#include <unistd.h>

class Terminal
{
public:
  Terminal()
  {
    init();
  }

  char getch()
  {
    struct pollfd fd{};
    fd.fd = STDIN_FILENO;
    fd.events = POLLIN;
    fd.revents = 0;
    const auto ret = poll(&fd, 1, 0);
    if (ret > 0 && ((fd.revents & POLLIN) != 0)) {
      char buff[100];
      auto ret_read = read(STDIN_FILENO, buff, 12);
      std::cout << "Read " << ret_read << std::endl;

      return buff[0];
    }

    return 'A';
  }

  ~Terminal()
  {
    destroy();
  }

private:
  void init()
  {
    tcgetattr(0, &old_); /* grab old terminal i/o settings */
    current_ = old_; /* make new settings same as old settings */
    current_.c_lflag &= ~ICANON; /* disable buffered i/o */
    current_.c_lflag &= ~ECHO; /* set no echo mode */
    tcsetattr(0, TCSANOW, &current_); /* use these new terminal i/o settings now */
  }

  void destroy()
  {
    tcsetattr(0, TCSANOW, &old_);
  }

private:
  struct termios old_, current_;
};
