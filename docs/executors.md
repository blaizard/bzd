# Executors

An executor is a group of computation units (cores) that can share workload between each others.
They share the same work queue and a component gets executed on an arbitrary core, unless constraints are defined for specific cores.
```
[ executor ] 1 <- * [ core ]
```

Cores can be assigned with a maximum load or other specific atrtibutes.

```bdl
// A workload will run on either core0 or core1.
composition
{
   core0 = FreeRTOS(stackSize = 4000, load = 20%);
   core1 = FreeRTOS(stackSize = 10000);
   esp32 = Executor(core0, core1);
}
```

Sometimes, for portability purpose, you might want to inject the knowledge of cores at runtime.
To do so, a platform specific executor can be made, abstracting the notion of cores.
For example, a linux machine can be composed as follow:
```bdl
composition
{
   linux = LinuxExecutor;
}
```

Multiple executors can be created and deployed on the same process/binary, this can be usefull to have different task priorities for example.

## Association

To associate a group of entities to an executor, this can be done at the composition level.
For example, to instanciate a component and execute it on the `esp32` executor, it can be done as follow:

```bdl
composition [executor(esp32)]
{
   comp1 = Component;
}
```
