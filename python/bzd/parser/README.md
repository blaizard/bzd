# Parser / Visitor

This library provides helpers to parse and visit text-based documents.

## Parser

The parser uses a state machine of regular expression to match entities and generates metadata that can then be used together with the visitor class.

Parsed data can also be serialized.

## Visitor

The visitor takes a parsed `sequence` into argument and goes through all entities to return an object.
