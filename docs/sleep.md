# Sleep

## Concept ideas

### Sleep manager task

Use a service task to monitor wether an executor should sleep.

The problem with this design is that the task would run all the time, even in heavy load operation,
stealing some cycles.
Also if using multiple executors, since this is happening inside the executor, it is hard to inject
the information of the others within the task.

### Manually sleep from application

This can be done and would be the easy way. However, this is bringing a platform specific
concept into a generic application.
Doing it implicitly will move this on the platform side.

### Idle task

When the executor has only services tasks running, it would exit and run an idle task.
```
while true:
    executor.run();
    if executor.isShutdown():
        break
    idle(executor);
```

The idle task is platform dependant and could look like this:
```
def idle(executor):
    if executor.sleepRequested():
        disableGlobalInterruptsScope();
        if executor.sleepRequested():
            sleep();
    else:
        wait(1ms);
```

The advantage with such design is that it decouuples the platform specific part. Also
if there are multiple executors, the logic to go to sleep could incorporate them all.
