# Executors

An executor is a group of computation units (cores) that can share workload between each others.
They share the same work queue.
```
[ executor ] 1 <- * [ core ]
```

The workload gets assigned to a core with constraint matching. For example, if a workloadd is tagged with `critical`, only cores with the same tag can run the workload.
Also cores can be assigned with a maximum load or other specific atrtibutes.

```bdl
executor esp32
{
   FreeRTOS core0(4000, 0) [load = 20%];
   FreeRTOS core1(10000, 1);
}
```
