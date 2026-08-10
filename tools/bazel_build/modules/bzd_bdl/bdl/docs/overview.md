# BDL

BDL or (Bzd Description Language) is a DSL (Domain Specific Language) for the bzd framework.

## Glossary

| Name        | Description                                                                                               |
| ----------- | --------------------------------------------------------------------------------------------------------- |
| Application | A runnable, typically a method. Its lifetime dictates the lifetime of the overall application.            |
| Service     | Similar to an application but its lifetime does not affect the rest of the application.                   |
| Workload    | An application or a service.                                                                              |
| Core        | A unit of execution, one or multiple cores that share the same memory compose an executor.                |
| Executor    | This is the minimal scheduling unit, it may contain multiple workloads and may execute on multiple cores. |
| Platform    | Implicit dependencies that a binary relies on.                                                            |
| Target      | A group of executors and a single platform aimed at creating a single binary.                             |
| System      | A group of targets that constitute a complete deployable system.                                          |
| Gateway     | Ports or connection endpoints for an executor, that aims at linking executors together.                   |
| Buses       | Connection between executors.                                                                             |

## Use Cases

The following use cases are covered by this language:

- Provide a language to describe distributed systems, regardless of the hardware.
- Reduce boilerplate code for interfaces while keeping options to the user without compromising on performance.
- Enforce input/output constraints (contracts).
