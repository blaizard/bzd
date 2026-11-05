# Logger

## Design

The logger should be 2 parts:

frontend + backend

the frontend should be non blocking, with an extra async api flush() to flush all the logs if we want to wait for it.

frontend serialize the records. The records are a unique ID corresponding to the raw static string, and the rest of the arguments are data that can be serialized.
Serilization of the data can be customized via a trait, if not present, it can use the typical serialization to string used by core::fmt
in otherword, a record is a destructured log that can be Send.

the backend is a simple circular queue that is filled with the records, and the backend deals with each entry in a different task/component.

The implementation is split as follows:

- `Logger` and its macros are the frontend. They serialize records into the queue.
- `LoggerProducer` is the frontend handle to the backend queue. It does not take the backend component lock.
- `LoggerBackend` owns the queue and runs a service that drains it to a specialization.
- The backend consumes records in place (`peek` then `commit`), without copying them out of the ring.
- A specialization (for example `OStreamLoggerSink`) implements `bzd.logger.Sink` and only writes/flushes the bytes.
