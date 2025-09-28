# Nodes Utilities

## Export

Export data from nodes.

### Usage Examples

```bash
bazel run //apps/artifacts/plugins/nodes/bin:export -- --output "$(pwd)/temp" --rename "{key[0]}/2024.csv" --level 3 --after 2024-01-01 --before 2025-01-01 https://datalocal.blaizard.com/view/nodes_keep/stocks/data/ohlc
```
