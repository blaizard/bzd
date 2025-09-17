# Data Scrappers

## Prerequesitites:

Run the Artifacts server first.

```bash
bazel run apps/artifacts/backend --config=dev -- --port 8081
```

## S&P 500

```bash
bazel run //apps/trader_python/recording:sp500 -- indexes
```
