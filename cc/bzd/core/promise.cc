#include "bzd/core/promise.h"

#include "bzd/core/assert.h"
#include "bzd/core/task.h"

void bzd::interface::Promise::setPending(bzd::NonOwningList<bzd::interface::Promise>& list)
{
	bzd::assert::isTrue(task_, "Promise is not associated to a task.");
	const auto result = list.pushFront(*this);
	bzd::assert::isTrue(result.hasValue(), "Cannot associate promise with list.");
	task_->setPending();
}

void bzd::interface::Promise::setActive(bzd::NonOwningList<bzd::interface::Promise>& list)
{
	bzd::assert::isTrue(task_, "Promise is not associated to a task.");
	const auto result = list.pop(*this);
	bzd::assert::isTrue(result.hasValue(), "The promise couldn't be detached.");
	task_->setActive();
}
