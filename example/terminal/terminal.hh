#pragma once

#include "cc/bzd.hh"
#include "example/terminal/composition.hh"

namespace example {

bzd::Async<> run(bzd::OStream& out, bzd::IStream& in);

}
