# Executors

An executor is a group of computation units (cores) that can share workloads between each others.
They share the same work queue and a component gets executed on an arbitrary core.

```mermaid
classDiagram
   Executor "1" --> "*" Core
```

Cores can be assigned with a maximum load or other specific attributes (to be implemented).

```bdl
// A workload will run on either core0 or core1.
composition
{
   core0 = FreeRTOS(stackSize = 4000, load = 20%);
   core1 = FreeRTOS(stackSize = 10000);
   esp32 = Executor(core0, core1) [executor];
}
```

Sometimes, for portability purpose, you might want to inject the knowledge of cores at runtime.
To do so, a platform specific executor can be made, abstracting the notion of cores.
For example, a linux machine can be composed as follow:

```bdl
composition
{
   linux = LinuxExecutor [executor];
}
```

Multiple executors can be created and deployed on the same process/binary, this can be useful to have different task priorities for example.

## Default executor

When the system only has a single executor, it is automatically assigned to all workloads, therefore there is no need for specific assignment.

## Association

If the system contains multiple executor, however, it is necessary to define where workloads are running.

This can be done in 2 different ways. By either specifying it at component instantiation, This is done with the `executor` contract.

```bdl
composition
{
   comp1 = Component [executor(linux)];
}
```

or separately from the component, using the builtin function `bind`, to bind a fqn or a regular expression to an executor, for example:

```bdl
composition
{
   bind(/comp1.*/) [executor(linux)];
}
```

This tells that all fqn starting with `comp1` will be assigned to the executor `linux`.
