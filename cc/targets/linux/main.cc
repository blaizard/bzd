int main()
{
	extern bool runExecutor() noexcept;
	if (runExecutor())
	{
		return 0;
	}
	return 1;
}
