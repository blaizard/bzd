# Logger

Leveled logging to an output stream.

## Key Points

- **Levels** - `error`, `warning`, `info` and `debug`.
- **Formatting** - uses the `_csv` string literal and the `toStream` pattern.
- **Filtering** - the minimum displayed level can be configured.

## Usage

```c++
bzd::Logger logger{out};
logger.error("Failed to open '{}'."_csv, path).sync();
logger.warning("Retrying...").sync();
logger.info("Started.").sync();
```
