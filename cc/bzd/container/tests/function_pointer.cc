#include "bzd/container/function_pointer.h"

#include "cc_test/test.h"

namespace {
class Dummy
{
public:
	Dummy(int val) : val_{val} {}

	void callNoArgsAdd1() { ++val_; }
	void callNoArgsAdd3() { val_ += 3; }

	void callSet(int val) { val_ = val; }

	int callSetAndReturn(int val) { val_ = val; return val_; }

	int getValue() const { return val_; }

private:
	int val_;
};
} // namespace

TEST(ContainerFunctionPointer, classMemberSimple)
{
	Dummy dummy{10};
	bzd::FunctionPointer<void(void)> ptr{dummy, &Dummy::callNoArgsAdd1};
	EXPECT_EQ(dummy.getValue(), 10);
	ptr();
	EXPECT_EQ(dummy.getValue(), 11);
}

TEST(ContainerFunctionPointer, classMemberWithArgs)
{
	Dummy dummy{-1};
	bzd::FunctionPointer<void(int)> ptr{dummy, &Dummy::callSet};
	EXPECT_EQ(dummy.getValue(), -1);
	ptr(15);
	EXPECT_EQ(dummy.getValue(), 15);
}

TEST(ContainerFunctionPointer, classMemberWithArgsAndReturn)
{
	Dummy dummy{882};
	bzd::FunctionPointer<int(int)> ptr{dummy, &Dummy::callSetAndReturn};
	EXPECT_EQ(dummy.getValue(), 882);
	EXPECT_EQ(ptr(3), 3);
	EXPECT_EQ(dummy.getValue(), 3);
}

TEST(ContainerFunctionPointer, classMemberTags)
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
