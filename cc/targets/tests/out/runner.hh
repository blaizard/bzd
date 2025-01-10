#pragma once

#include "cc/bzd.hh"
#include "cc/bzd/test/test.hh"
#include "cc/targets/tests/out/composition.hh"

namespace tests {

class CustomRunner : public bzd::test::Runner
{
public:
	CustomRunner(auto& context) : out{context.config.out} {}

public:
	bzd::OStream& out;
};

} // namespace tests
