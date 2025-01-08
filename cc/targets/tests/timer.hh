#pragma once

#include "cc/bzd.hh"
#include "cc/bzd/test/test.hh"
#include "cc/targets/tests/composition.hh"

namespace tests {

class CustomRunner : public bzd::test::Runner
{
public:
	CustomRunner(auto& context) : timer{context.config.timer} {}

public:
	bzd::Timer& timer;
};

} // namespace tests
