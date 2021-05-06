#include "bzd/container/function_view.h"

#include "cc_test/test.h"

namespace {
class Dummy
{
public:
	Dummy(int val) : val_{val} {}

	void callNoArgsAdd1() { ++val_; }
	void callNoArgsAdd3() { val_ += 3; }

	void callSet(int val) { val_ = val; }

	int callSetAndReturn(int val)
	{
		val_ = val;
		return val_;
	}

	int getValue() const { return val_; }

	virtual void callVirtualAdd(int val) { val_ += val; }

protected:
	int val_;
};

class DummyChild : public Dummy
{
public:
	using Dummy::Dummy;
	virtual void callVirtualAdd(int) override { val_ = 42; }
};

void callNoArgs() {}

void callArgsAdd1(int& val)
{
	++val;
}

int callSquare(int val)
{
	return val * val;
}

} // namespace

TEST(ContainerFunctionView, classMemberSimple)
{
	Dummy dummy{10};
	bzd::FunctionView<void(void)> ptr{dummy, &Dummy::callNoArgsAdd1};
	EXPECT_EQ(dummy.getValue(), 10);
	ptr();
	EXPECT_EQ(dummy.getValue(), 11);

	auto ptrCopy = ptr;
	ptrCopy();
	EXPECT_EQ(dummy.getValue(), 12);
	auto ptrMove = bzd::move(ptr);
	ptrMove();
	EXPECT_EQ(dummy.getValue(), 13);
}

TEST(ContainerFunctionView, classMemberWithArgs)
{
	Dummy dummy{-1};
	bzd::FunctionView<void(int)> ptr{dummy, &Dummy::callSet};
	EXPECT_EQ(dummy.getValue(), -1);
	ptr(15);
	EXPECT_EQ(dummy.getValue(), 15);

	auto ptrCopy = ptr;
	ptrCopy(2);
	EXPECT_EQ(dummy.getValue(), 2);
	auto ptrMove = bzd::move(ptr);
	ptrMove(9);
	EXPECT_EQ(dummy.getValue(), 9);
}

TEST(ContainerFunctionView, classMemberWithArgsAndReturn)
{
	Dummy dummy{882};
	bzd::FunctionView<int(int)> ptr{dummy, &Dummy::callSetAndReturn};
	EXPECT_EQ(dummy.getValue(), 882);
	EXPECT_EQ(ptr(3), 3);
	EXPECT_EQ(dummy.getValue(), 3);

	auto ptrCopy = ptr;
	EXPECT_EQ(ptrCopy(2), 2);
	auto ptrMove = bzd::move(ptr);
	EXPECT_EQ(ptrMove(9), 9);
}

TEST(ContainerFunctionView, classMemberVirtual)
{
	// Base class
	Dummy dummy{-1};
	bzd::FunctionView<
	void(int)> ptr{dummy, &Dummy::callVirtualAdd};
	EXPECT_EQ(dummy.getValue(), -1);
	ptr(15);
	EXPECT_EQ(dummy.getValue(), 14);

	// Child class
	DummyChild child{32};
	bzd::FunctionView<void(int)> ptrChild{static_cast<Dummy&>(child), &Dummy::callVirtualAdd};
	EXPECT_EQ(child.getValue(), 32);
	ptrChild(15);
	EXPECT_EQ(child.getValue(), 42);

	auto ptrChildCopy = ptrChild;
	ptrChildCopy(24);
	EXPECT_EQ(child.getValue(), 42);
	auto ptrChildMove = bzd::move(ptrChild);
	ptrChildMove(24);
	EXPECT_EQ(child.getValue(), 42);
}

TEST(ContainerFunctionView, classMemberTags)
{
	Dummy dummy1{10};
	Dummy dummy2{45};
	bzd::FunctionView<void(void)> ptr1{dummy1, &Dummy::callNoArgsAdd1};
	bzd::FunctionView<void(void), struct A> ptr2{dummy2, &Dummy::callNoArgsAdd3};
	EXPECT_EQ(dummy1.getValue(), 10);
	EXPECT_EQ(dummy2.getValue(), 45);
	ptr1();
	ptr2();
	EXPECT_EQ(dummy1.getValue(), 11);
	EXPECT_EQ(dummy2.getValue(), 48);
}

TEST(ContainerFunctionView, functionNoArgs)
{
	bzd::FunctionView<void(void)> ptr{callNoArgs};
	ptr();
}

TEST(ContainerFunctionView, functionArgs)
{
	bzd::FunctionView<void(int&)> ptr{&callArgsAdd1};
	int a = 6;
	ptr(a);
	EXPECT_EQ(a, 7);
}

TEST(ContainerFunctionView, functionReturn)
{
	bzd::FunctionView<int(int)> ptr{callSquare};
	EXPECT_EQ(ptr(9), 81);
}

TEST(ContainerFunctionView, objectFunctionNoArgs)
{
	int a = 0;
	bzd::Function<void(void)> fct{[&]() { a += 42; }};
	bzd::FunctionView<void(void)> ptr{fct};
	EXPECT_EQ(a, 0);
	fct();
	EXPECT_EQ(a, 42);
	ptr();
	EXPECT_EQ(a, 84);
}

TEST(ContainerFunctionView, objectFunctionArgsReturn)
{
	int a = 2;
	bzd::Function<int(int)> fct{[&](int b) -> int { return b * b + a; }};
	bzd::FunctionView<int(int)> ptr{fct};
	EXPECT_EQ(a, 2);
	EXPECT_EQ(fct(5), 27);
	EXPECT_EQ(ptr(3), 11);
}

TEST(ContainerFunctionView, lambdaNoArgs)
{
	int a = 0;
	auto lambda = [&a]() { ++a; };
	bzd::FunctionView<void(void)> ptr{lambda};
	EXPECT_EQ(a, 0);
	lambda();
	EXPECT_EQ(a, 1);
	ptr();
	EXPECT_EQ(a, 2);
}

TEST(ContainerFunctionView, lambdaArgsReturn)
{
	auto lambda = [](int a) { return a * a; };
	bzd::FunctionView<int(int)> ptr{lambda};
	EXPECT_EQ(lambda(7), 49);
	EXPECT_EQ(ptr(3), 9);

	auto ptrCopy = ptr;
	EXPECT_EQ(ptrCopy(2), 4);
	auto ptrMove = bzd::move(ptr);
	EXPECT_EQ(ptrMove(9), 81);
}
