#include "cc/bzd/test/test.hh"
#include "tools/bdl/generators/tests/interface_inheritance/interface.hh"

namespace bzd::test {

class ImplementationPartial
{
public:
	bzd::Int32 add(const bzd::Int32& a, const bzd::Int32& b) noexcept { return a + b; }
};

class Implementation
	: ImplementationPartial
	, public MyInterface<Implementation>
{
public:
	using ImplementationPartial::add;
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
	static_assert(bzd::typeTraits::isSame<decltype(interface), bzd::test::MyInterface<bzd::test::Implementation>&>,
				  "Must have the type of the interface.");
	ASSERT_EQ(interface.add(2, 3), 5);
}
