#include "cc/bzd/container/function_ref.hh"

#include "cc/bzd/test/test.hh"

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

	void overload(int val) { val_ = val; }
	void overload(float) {}
	void overload(char*) {}

	int methodConst(int a) const { return a; }

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

void callArgsAdd1(int& val) { ++val; }

int callSquare(int val) { return val * val; }

} // namespace

TEST(ContainerFunctionRef, classMemberSimple)
{
	Dummy dummy{10};

	auto ptr = bzd::FunctionRef<void(void)>::toMember<Dummy, &Dummy::callNoArgsAdd1>(dummy);
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

TEST(ContainerFunctionRef, classMemberWithArgs)
{
	Dummy dummy{-1};
	auto ptr = bzd::FunctionRef<void(int)>::toMember<Dummy, &Dummy::callSet>(dummy);
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

TEST(ContainerFunctionRef, classMemberWithArgsAndReturn)
{
	Dummy dummy{882};
	auto ptr = bzd::FunctionRef<int(int)>::toMember<Dummy, &Dummy::callSetAndReturn>(dummy);
	EXPECT_EQ(dummy.getValue(), 882);
	EXPECT_EQ(ptr(3), 3);
	EXPECT_EQ(dummy.getValue(), 3);

	auto ptrCopy = ptr;
	EXPECT_EQ(ptrCopy(2), 2);
	auto ptrMove = bzd::move(ptr);
	EXPECT_EQ(ptrMove(9), 9);
}

TEST(ContainerFunctionRef, Overloads)
{
	Dummy dummy{882};
	auto ptr = bzd::FunctionRef<void(int)>::toMember<Dummy, &Dummy::overload>(dummy);
	ptr(3);
	EXPECT_EQ(dummy.getValue(), 3);

	auto ptrFloat = bzd::FunctionRef<void(float)>::toMember<Dummy, &Dummy::overload>(dummy);
	ptrFloat(5);
	EXPECT_EQ(dummy.getValue(), 3);

	ptr(7);
	EXPECT_EQ(dummy.getValue(), 7);
}

TEST(ContainerFunctionRef, Const)
{
	const Dummy dummy{882};
	auto ptr = bzd::FunctionRef<int(int)>::toMember<Dummy, &Dummy::methodConst>(dummy);
	EXPECT_EQ(ptr(3), 3);
}

TEST(ContainerFunctionRef, classMemberVirtual)
{
	// Base class
	Dummy dummy{-1};
	auto ptr = bzd::FunctionRef<void(int)>::toMember<Dummy, &Dummy::callVirtualAdd>(dummy);
	EXPECT_EQ(dummy.getValue(), -1);
	ptr(15);
	EXPECT_EQ(dummy.getValue(), 14);

	// Child class
	DummyChild child{32};
	auto ptrChild = bzd::FunctionRef<void(int)>::toMember<Dummy, &Dummy::callVirtualAdd>(static_cast<Dummy&>(child));
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

TEST(ContainerFunctionRef, multipleWithSameType)
{
	Dummy dummy1{10};
	Dummy dummy2{45};
	auto ptr1 = bzd::FunctionRef<void(void)>::toMember<Dummy, &Dummy::callNoArgsAdd1>(dummy1);
	auto ptr2 = bzd::FunctionRef<void(void)>::toMember<Dummy, &Dummy::callNoArgsAdd3>(dummy2);
	EXPECT_EQ(dummy1.getValue(), 10);
	EXPECT_EQ(dummy2.getValue(), 45);
	ptr1();
	ptr2();
	EXPECT_EQ(dummy1.getValue(), 11);
	EXPECT_EQ(dummy2.getValue(), 48);
}

TEST(ContainerFunctionRef, functionNoArgs)
{
	bzd::FunctionRef<void(void)> ptr{callNoArgs};
	ptr();
}

TEST(ContainerFunctionRef, functionArgs)
{
	bzd::FunctionRef<void(int&)> ptr{&callArgsAdd1};
	int a = 6;
	ptr(a);
	EXPECT_EQ(a, 7);
}

TEST(ContainerFunctionRef, functionReturn)
{
	bzd::FunctionRef<int(int)> ptr{callSquare};
	EXPECT_EQ(ptr(9), 81);
}

TEST(ContainerFunctionRef, constLambdaNoArgs)
{
	static int a = 0;
	const auto lambda = []() { ++a; };
	bzd::FunctionRef<void(void)> ptr{lambda};
	EXPECT_EQ(a, 0);
	lambda();
	EXPECT_EQ(a, 1);
	ptr();
	EXPECT_EQ(a, 2);
}

TEST(ContainerFunctionRef, mutableLambdaNoArgs)
{
	auto lambda = [a = 0]() mutable -> int {
		++a;
		return a;
	};
	bzd::FunctionRef<int(void)> ptr{lambda};
	EXPECT_EQ(lambda(), 1);
	EXPECT_EQ(lambda(), 2);
}

TEST(ContainerFunctionRef, lambdaNoArgs)
{
	int a = 0;
	auto lambda = [&a]() { ++a; };
	bzd::FunctionRef<void(void)> ptr{lambda};
	EXPECT_EQ(a, 0);
	lambda();
	EXPECT_EQ(a, 1);
	ptr();
	EXPECT_EQ(a, 2);

	// This should not compile, we should not be able to use an r-value.
	// bzd::FunctionRef<void(void)> ptrRValue{[&a](){++a;}};
	// ptrRValue();
}

TEST(ContainerFunctionRef, lambdaArgsReturn)
{
	auto lambda = [](int a) { return a * a; };
	bzd::FunctionRef<int(int)> ptr{lambda};
	EXPECT_EQ(lambda(7), 49);
	EXPECT_EQ(ptr(3), 9);

	auto ptrCopy = ptr;
	EXPECT_EQ(ptrCopy(2), 4);
	auto ptrMove = bzd::move(ptr);
	EXPECT_EQ(ptrMove(9), 81);
}

TEST(ContainerFunctionRef, lambdaPointer)
{
	int a = 42;
	auto lambda = [&a](int* pointer) { *pointer = a; };
	bzd::FunctionRef<void(int*)> ptr{lambda};
	int b = 0;
	lambda(&b);
	EXPECT_EQ(b, 42);
}
