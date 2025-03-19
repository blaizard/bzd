# Record Class Documentation

## Overview

The `Record` class implements an on-disk, time-series data storage system. It's designed to efficiently store and retrieve records, which are essentially time-stamped data payloads. This system is particularly useful for applications requiring persistent, ordered data storage, such as event logs, message queues, or data streams.

## Key Features

- Persistent, ordered data storage.
- Efficient time-series data handling.
- Support for multiple data streams (storages).
- Automatic record file management.
- Data integrity checks.
- Thread safety.

## Software Architecture

The architecture revolves around the concept of "storages." A storage is a directory on the file system that contains a collection of record files. Each record file is named with a numerical tick value, indicating its position in the time series. This allows for ordered retrieval of records.

- **Storages:**
  - The `Record` class can manage multiple storages, each representing a separate data stream.
  - Each storage is a subdirectory within a base path provided during initialization.
  - The default storage is named "main."
- **Record Files:**
  - Record files store data in a JSON-like format.
  - Each file contains a list of entries, where each entry is a tuple: `[tick, payload, payloadSize, remoteTick]`.
  - The `tick` is a unique, monotonically increasing number across all storages representing the time of the entry.
  - `payload` is the data itself, stored as a JSON string.
  - `payloadSize` is the size of the serialized payload string.
  - `remoteTick` is a tick value from another system that relates to the data in the payload.
  - Record files have a maximum size limit, preventing them from growing indefinitely.
- **Tick System:**
  - The system uses a tick-based approach to order records.
  - Ticks are monotonically increasing integers.
  - This allows for efficient retrieval of records in chronological order.
- **Locks:**
  - Each storage utilizes a lock to control access to the storage files, ensuring thread safety.

## Conceptual Explanation

The core concept is to break down a continuous stream of data into manageable, ordered files. When new data arrives, it's assigned a tick value and appended to the appropriate record file. If a record file reaches its maximum size, a new file is created with the next available tick. Old files are removed when the total storage size exceeds the max size.

- **Data Organization:**
  - Data is stored as a sequence of records, where each record is a file.
  - The files are ordered by their tick values, ensuring chronological order.
  - The class manages the creation, appending, and deletion of record files.
- **Data Integrity:**
  - The system includes a sanitization process that checks the integrity of record files.
  - This process verifies that ticks are in order, file sizes are within limits, and data is valid.
  - Invalid files are deleted.
- **Reading Data:**
  - Data is read by iterating through the record files in tick order.
  - The `read` method provides a generator that yields data entries in chronological order.
  - The read method is designed to be able to read data from multiple storages at the same time, merging the data based on the tick values.
- **Writing Data:**
  - Data is written by appending new entries to the last record file.
  - If the last record file is full, a new file is created.
  - The `write` method handles the creation and appending of record entries.
  - Unlike reading, writing is done toward a specified storage.
