#include "cc/bzd/test/test.hh"
#include "cc/targets/tests/out/runner.hh"

TEST_ASYNC(Out, Print)
{
	auto& out = test.template getRunner<tests::CustomRunner>().out;
	co_await !bzd::print(out, "\nTEST MESSAGE 95485743\n"_csv);

	co_return {};
}
