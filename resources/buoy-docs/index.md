---
title: "GitKit"
summary: "Inspect local Git HEAD, status, and diffs without shelling out or mutating repositories."
category: "Overview"
order: 0
keywords: ["git", "status", "diff", "repository", "libgit2"]
related: ["getting-started.md", "api.md", "typed-api.md"]
---

# GitKit

GitKit exposes owned Buoy snapshots backed by embedded libgit2. Consumers do not need the Git CLI and do not invoke a shell. The API is intentionally read-only: HEAD metadata, recursive status including untracked paths, and staged/unstaged diffs.

## Ownership and threading

GitRepository owns a native handle. Close it explicitly or allow ARC cleanup; Close is idempotent. Head and status snapshots own their strings and remain valid after the repository closes. A closed repository rejects further operations.

Use one repository instance from one thread or synchronize externally. Scans and diffs can block; use a worker in an IDE. Separate calls do not create one atomic view when another process changes the repository concurrently.

## Distribution limits

Ship a native runtime slice for each supported deployment target. Compiler checks for six targets do not prove those binaries are present. There are no staging, commits, checkout, reset, fetch, credentials, blame/history, submodule recursion, or SHA-256 repository APIs.

## Read next

- [Overview](index.md)
- [Getting started](getting-started.md)
- [API reference](api.md)
