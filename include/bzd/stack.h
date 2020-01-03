#pragma once

#include "bzd/interface/stack.h"
#include "bzd/types.h"

namespace bzd {
template <const SizeType N>
class Stack : public interface::Stack
{
  public:
	Stack() : interface::Stack(data_, N) {}

  private:
	UInt8Type data_[N];
};
}  // namespace bzd
