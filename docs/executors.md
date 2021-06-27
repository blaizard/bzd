# Executors

An executor is a group of computation units (cores) that can share workload between each others.
They share the same work queue and a component gets executed on an arbitrary core, unless constraints are defined for specific cores.
```
[ executor ] 1 <- * [ core ]
```

Cores can be assigned with a maximum load or other specific atrtibutes.

```bdl
// A workload will run on either core0 or core1.
composition esp32
{
   core0 = FreeRTOS(4000, 0) [load = 20%];
   core1  = FreeRTOS(10000, 1);
   executor = Executor(core0, core1);
}
```

Sometimes, for portability purpose, you might want to inject the knowledge of cores at runtime.
To do so, a platform specific executor can be made, abstracting the notion of cores.
For example, a linux machine can be composed as follow:
```bdl
composition linux
{
   executor = LinuxExecutor;
}
```
## Association

To associate a group of entities to an executor, this can be done at the composition level, as such:

```bdl
composition [executor = esp32]
{
   comp1 = Component;
}
```

or for a single entity, with the `map` keyword:

```bdl
composition
{
   map(comp1) [executor = esp32];
}
```
