#include "cc/bzd/utility/scope_guard.hh"

#include "cc/bzd/test/test.hh"

template <class T>
void changeScope(T&& s)
{
	[[maybe_unused]] T scope{bzd::forward<T>(s)};
}

TEST(ScopeGuard, Base)
{
	int counter = 0;
	EXPECT_EQ(counter, 0);
	{
		bzd::ScopeGuard scope{[&counter]() { ++counter; }};
	}
	EXPECT_EQ(counter, 1);
	{
		bzd::ScopeGuard scope{[&counter]() { ++counter; }};
		changeScope(bzd::move(scope));
		EXPECT_EQ(counter, 2);
	}
	EXPECT_EQ(counter, 2);
}
