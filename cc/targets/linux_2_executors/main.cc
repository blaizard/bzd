#include <atomic>
#include <thread>

int main()
{
	extern bool runExecutor() noexcept;
	extern bool runSecondExecutor() noexcept;
	std::atomic<bool> isSuccess{true};

	std::thread threadExecutor{[&isSuccess] {
		if (!runExecutor())
		{
			isSuccess = false;
		}
	}};
	std::thread threadSecondExecutor{[&isSuccess] {
		if (!runSecondExecutor())
		{
			isSuccess = false;
		}
	}};

	threadExecutor.join();
	threadSecondExecutor.join();

	return (isSuccess) ? 0 : 1;
}
