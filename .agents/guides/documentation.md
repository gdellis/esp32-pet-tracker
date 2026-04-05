# Documentation Guide

## Purpose

Create clear, maintainable documentation for projects, APIs, and processes.

## Types of Documentation

| Type | Purpose | Location |
| ----- | ------- | -------- |
| README | Project overview, quick start | `README.md` |
| API Docs | Function/class references | Inline (docstrings) or `docs/api/` |
| Architecture | System design, decisions | `docs/architecture.md` |
| Contributing | How to contribute | `CONTRIBUTING.md` |
| Changelog | Version history | `CHANGELOG.md` |

## README Structure

```markdown
# Project Name

Brief description of what the project does.

## Features

- Key feature 1
- Key feature 2

## Installation

Step-by-step installation instructions.

## Usage

Basic usage examples.

## Documentation

Links to additional docs.

## Contributing

How to contribute.

## License

License information.
```

## API Documentation (Rust)

```rust
/// Calculate the sum of two integers.
///
/// # Arguments
///
/// * `a` - First integer.
/// * `b` - Second integer.
///
/// # Returns
///
/// The sum of a and b.
fn calculate_sum(a: i32, b: i32) -> i32 {
    a + b
}
```

## File Operations with Error Context

```rust
let metadata_path = backup_path.with_extension("ab_metadata.json");
let metadata_json = serde_json::to_string_pretty(&metadata)
    .map_err(|e| AppError::Parse(e.to_string()))?;
```

## Architecture Decision Records (ADRs)

Store in `docs/adr/`:

```markdown
# ADR-001: Use PostgreSQL for Primary Database

## Status

Accepted

## Context

Need a relational database that supports complex queries and transactions.

## Decision

Use PostgreSQL as the primary database.

## Consequences

- Requires Docker or local PostgreSQL installation
- Team needs SQL knowledge
- Good tooling and documentation available
```

## Best Practices

- **Keep it updated**: Outdated docs are worse than no docs
- **Write for the audience**: Developers? Users? Both?
- **Use examples**: Show, don't just tell
- **Be concise**: Remove unnecessary words
- **Link don't duplicate**: Reference other docs instead of copying
- **Update CHANGELOG.md**: Add entries under `[Unreleased]` for all notable changes (fixes, features, breaking changes)
