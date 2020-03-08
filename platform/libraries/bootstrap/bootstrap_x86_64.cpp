#include <signal.h>
#include <execinfo.h>

void sigHandler(const int /*sig*/, siginfo_t* /*info*/, void* /*secret*/)
{
	std::cout << "HEEERRRRR" << std::endl;
	char** strings;
	size_t i, size;
	enum Constexpr { MAX_SIZE = 1024 };
	void* array[MAX_SIZE];
	size = backtrace(array, MAX_SIZE);
	strings = backtrace_symbols(array, size);
	for (i = 0; i<size; ++i)
		std::cout << strings[i] << std::endl;
	free(strings);
}

bool installBootstrap()
{
	struct sigaction sa{};
	sa.sa_sigaction = static_cast<void (*)(int, siginfo_t*, void*)>(sigHandler);
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART | SA_SIGINFO;
	for (const auto& signal : {SIGSEGV, SIGFPE, SIGILL, SIGSYS, 11})
	{
		sigaction(signal, &sa, nullptr);
	}
	return true;
}

static bool isBootstrapInstalled = installBootstrap();