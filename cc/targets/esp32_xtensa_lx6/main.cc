int main()
{
	extern bool execute() noexcept;
	execute();

	// Prevent the program to exit.
	while (true)
	{
	};

	return 0;
}
