int main()
{
	extern bool execute() noexcept;
	if (execute())
	{
		return 0;
	}
	return 1;
}
