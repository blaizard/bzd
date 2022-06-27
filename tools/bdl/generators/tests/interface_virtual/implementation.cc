#include "cc/bzd/test/test.hh"
#include "tools/bdl/generators/tests/interface_virtual/interface.hh"

namespace bzd::test {

class Implementation : public MyInterface
{
public:
	bzd::Int32Type add(const bzd::Int32Type& a, const bzd::Int32Type& b) noexcept override { return a + b; }
};

} // namespace bzd::test

TEST(Interface, Basic)
{
	bzd::test::Implementation impl{};
	ASSERT_EQ(impl.add(2, 3), 5);
}

TEST(Interface, Cast)
{
	bzd::test::Implementation impl{};
	auto& interface = bzd::Interface<"bzd.test.MyInterface">::cast(impl);
	static_assert(bzd::typeTraits::isSame<decltype(interface), bzd::test::MyInterface&>, "Must have the type of the interface.");
	ASSERT_EQ(interface.add(2, 3), 5);
}
