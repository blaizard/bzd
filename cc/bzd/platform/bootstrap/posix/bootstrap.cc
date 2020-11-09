#include <cstring>
#include <cxxabi.h>
#include <execinfo.h>
#include <iomanip>
#include <iostream>
#include <memory>
#include <signal.h>

namespace {
bool demangle(char* pBuffer, const size_t size, const char* const pSymbol) noexcept
{
	int status;
	try
	{
		const std::unique_ptr<char, decltype(&std::free)> demangled(abi::__cxa_demangle(pSymbol, 0, 0, &status), &std::free);
		std::strncpy(pBuffer, ((demangled && status == 0) ? demangled.get() : pSymbol), size);
	}
	catch (...)
	{
		// Ignore and return false
		return false;
	}
	return (size > 0 && pBuffer[0] != '\0');
}

void callStack(std::ostream& out) noexcept
{
	constexpr size_t MAX_STACK_LEVEL = 64;
	void* addresses[MAX_STACK_LEVEL];

	const int nbLevels = ::backtrace(addresses, MAX_STACK_LEVEL);
	const std::unique_ptr<char*, decltype(&std::free)> symbols(::backtrace_symbols(addresses, nbLevels), &std::free);

	for (int level = 0; level < nbLevels; ++level)
	{
		char* pSymbol = symbols.get()[level];
		const char* pSourcePath = nullptr;
		const char* pFunction = nullptr;
		const char* pOffset = nullptr;

		// Look for the source path
		{
			const auto pEnd = std::strchr(pSymbol, '(');
			if (pEnd)
			{
				*pEnd = '\0';
				pSourcePath = pSymbol;
				pSymbol = pEnd + 1;
			}
		}

		// Look for the function
		if (pSourcePath)
		{
			const auto pEnd = std::strchr(pSymbol, '+');
			if (pEnd)
			{
				*pEnd = '\0';
				pFunction = pSymbol;
				pSymbol = pEnd + 1;
			}
		}

		// Look for the offset
		if (pFunction)
		{
			const auto pEnd = std::strchr(pSymbol, ')');
			if (pEnd)
			{
				*pEnd = '\0';
				pOffset = pSymbol;
				pSymbol = pEnd + 1;
			}
		}

		// Print stack trace number and memory address
		out << "#" << std::dec << std::left << std::setfill(' ') << std::setw(3) << level << "0x" << std::setfill('0') << std::hex
			<< std::right << std::setw(16) << reinterpret_cast<uint64_t>(addresses[level]);

		char pBuffer[1024];
		if (pFunction)
		{
			if (demangle(pBuffer, sizeof(pBuffer), pFunction))
			{
				out << " in " << pBuffer;
				if (pOffset)
				{
					out << "+" << pOffset;
				}
			}
		}

		out << " (" << pSourcePath << ")";
		out << std::endl;
	}
}

void sigHandler(const int /*sig*/, siginfo_t* /*info*/, void* /*secret*/)
{
	callStack(std::cout);
}
} // namespace

bool installBootstrap()
{
	struct sigaction sa
	{
	};
	sa.sa_sigaction = static_cast<void (*)(int, siginfo_t*, void*)>(sigHandler);
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART | SA_SIGINFO;
	for (const auto& signal :
		 {SIGABRT, SIGALRM, SIGBUS,	 SIGCHLD, SIGCONT, SIGFPE,	SIGHUP,	 SIGILL,  SIGINT, SIGKILL, SIGPIPE, SIGQUIT,   SIGSEGV, SIGSTOP,
		  SIGTERM, SIGTSTP, SIGTTIN, SIGTTOU, SIGUSR1, SIGUSR2, SIGPOLL, SIGPROF, SIGSYS, SIGTRAP, SIGURG,	SIGVTALRM, SIGXCPU, SIGXFSZ})
	{
		sigaction(signal, &sa, nullptr);
	}
	return true;
}

static bool isBootstrapInstalled = installBootstrap();
