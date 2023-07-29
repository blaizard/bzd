#include "cc/bzd/utility/ranges/views/views.hh"

int main()
{
	for ([[maybe_unused]] auto c : "Hello" | bzd::ranges::drop(2))
	{
	}

	// Solve const char* as range.
	/*
	const char* rangeJoin[] = {"Hello", "You"};
	for ([[maybe_unused]] auto c : rangeJoin | bzd::ranges::join())
	{
	}
	*/

	return 0;
}
