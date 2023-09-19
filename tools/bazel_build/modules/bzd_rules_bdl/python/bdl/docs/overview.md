# BDL

BDL or (Bzd Description Language) is a DSL (Domain Specific Language) for the bzd framework.

## Glossary

| Name        | Description                                                                                                 |
| ----------- | ----------------------------------------------------------------------------------------------------------- |
| Application | A runnable, typically a method. Its lifetime dictates the lifetimme of the overall application.             |
| Service     | Simmilar to a service but it's lifetime is not affecting the rest of the application.                       |
| Worload     | An application or a service.                                                                                |
| Core        | A unit of execution, one or multiple cores that share the same memory compose an executor.                  |
| Executor    | This is the minimal scheduling unit, it maybe contain multiple workloads and may execute on multiple cores. |
| Platform    | Implicit dependencies that a binary relies on.                                                              |
| Target      | A group of executors and a single platform amed at creating a single binary.                                |
| System      | A group of targets that constitute a complete deployable system.                                            |
| Gateway     | Ports or connection endpoints for an executor, that aims at linking executors together.                     |
| Buses       | Connection between executors.                                                                               |

## Use Cases

The following use cases are covered by this language:

- Provide a language to discribe distributed systems, regardless of the hardware.
- Reduce boiler plate code for interfaces while keep options to the user wihtout compromising on performance.
- Enfore input/output constraints (contracts).
