# Power

- Coordinates deep sleep for the current target.
- Light sleep is not managed here: the platform executor runtime enters light sleep automatically when no task is runnable.
- Deep sleep powers down the CPU, RAM, and peripherals; memory is lost and the system restarts on wakeup.
- Deep-sleep requests are target-wide and visible to every executor.
- The first deep-sleep request wins.
- A deep-sleep request can specify a duration or wait for an external wake source.
- Each executor has one power handle, shared by that executor's components.
- Prevention is local to an executor: one executor preventing deep sleep does not prevent another executor from requesting it.
- Multiple components in the same executor can prevent deep sleep at the same time.
- Prevention lasts until the protected work completes or is cancelled.
- Prevention can be nested.
- A component can check whether its executor is going to deep sleep, taking prevention into account.
- An executor waiting for deep sleep is woken when deep sleep is requested or when prevention is released, then re-checks its own state.
- The platform performs the actual deep sleep after the executor finishes.
