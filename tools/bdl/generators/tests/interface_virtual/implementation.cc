#include "tools/bdl/generators/tests/interface_virtual/interface.hh"
#include "cc/bzd/test/test.hh"

namespace bzd::test {

class Implementation : public MyInterface
{
public:
    bzd::Int32Type add(const Int32Type& a, const Int32Type& b) noexcept override
    {
        return a + b;
    }
};

}

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
