#include "bzd/platform/core.h"

#include <pthread.h>

namespace bzd::platform {

void Core::init() noexcept {}

int pthread_attr_setstack(pthread_attr_t* attr, void* stackaddr, size_t stacksize);

} // namespace bzd::platform
