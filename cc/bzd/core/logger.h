namespace bzd::log {
enum class Level
{
	CRITICAL = 0,
	ERROR = 1,
	WARNING = 2,
	INFO = 3,
	DEBUG = 4
};

class Logger
{
public:
	/**
	 * Set the minimum logging level to be displayed.
	 */
	void setMinimumLevel(const Level level) noexcept;

	template <class... Args>
	constexpr void info(Args&&... args) noexcept
	{
		print(bzd::log::Level::INFO, bzd::forward<Args>(args)...);
	}

	static [[nodiscard]] Logger& getDefault() noexcept
	{
		static Logger logger;
		return logger;
	}

protected:
	/**
	 * Set the logger backend, by default a stub is in place.
	 * The logger backend is global for the whole application.
	 */
	static void setBackend();

private:
	Level minLevel{Level::INFO};
};

} // namespace bzd::log
