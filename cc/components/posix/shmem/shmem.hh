#include "cc/components/posix/shmem/interface.hh"

namespace bzd::platform::posix {

template <class Context>
class Shmem : public bzd::platform::Gateway<Shmem<Context>>
{
public:
	constexpr Shmem(Context& context) noexcept : context_{context} {}

	bzd::Async<> connect() noexcept
	{
		co_return {};
	}

private:
	Context& context_;
};

} // namespace bzd::platform::posix
