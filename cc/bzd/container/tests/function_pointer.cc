#include "bzd/container/function_pointer.h"

#include "cc_test/test.h"

namespace
{
class Dummy
{
public:
	Dummy(int val) : val_{val} {}

	void callNoArgsAdd1() { ++val_; }
	void callNoArgsAdd3() { val_ += 3; }

	int getValue() const { return val_; }

private:
	int val_;
};
}

TEST(ContainerFunctionPointer, simple)
{
	Dummy dummy1{10};
	Dummy dummy2{45};
	bzd::FunctionPointer<void(void)> ptr1{dummy1, &Dummy::callNoArgsAdd1};
	bzd::FunctionPointer<void(void), struct A> ptr2{dummy2, &Dummy::callNoArgsAdd3};
	EXPECT_EQ(dummy1.getValue(), 10);
	EXPECT_EQ(dummy2.getValue(), 45);
	ptr1();
	ptr2();
	EXPECT_EQ(dummy1.getValue(), 11);
	EXPECT_EQ(dummy2.getValue(), 48);
}
