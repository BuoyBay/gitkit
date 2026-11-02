# GitKit · 0.1.0

Read-only local Git inspection for an IDE's source-control sidebar and diff view.
`Import gitkit` exposes managed Buoy objects backed by embedded libgit2 1.9.7;
consumer applications do not invoke a shell or require the Git CLI.

## Build and use

Requires Buoy 0.72+, CMake 3.16+, a C11 compiler and libgit2 1.9.7 source.
Provide the libgit2 source directory explicitly to the included CMake project:

```sh
cmake -S native -B build/native -DLIBGIT2_SOURCE=/absolute/path/to/libgit2-1.9.7
cmake --build build/native --target buoy_gitkit
```

Use a target-appropriate CMake toolchain for cross builds. Install the resulting
bridge under the package's matching `runtime/<target>` directory, plus a Windows
import library under `link/<target>` where required, before sealing the package.
The [example](../examples/gitkit_demo.bui) takes a repository directory.

| Object/member | Behavior |
| --- | --- |
| `GitRepository(directory, discover=True)` | Open a local repository, optionally searching parents; failures throw |
| `RepositoryPath`, `WorkingDirectory`, `IsBare` | Git directory, worktree directory and repository kind |
| `Head()` | Owned `GitHead` snapshot |
| `Status()` | Owned array of `GitStatusEntry` snapshots |
| `DiffBytes(staged=False, contextLines=3, maxBytes=33554432)` | Unified patch bytes |
| `DiffText(...)` | UTF-8 patch text; invalid encoding throws |
| `Close()` | Idempotent native-owner release |

`GitStatusEntry` retains literal repository-relative paths and rename paths.
HEAD snapshots distinguish unborn and detached states. Missing/corrupt repositories
and indexes raise errors rather than reporting a clean worktree; bare repositories
can expose HEAD while operations requiring a worktree may fail.

## Scope and ownership

There are no staging, checkout, commit, reset, fetch or other mutation APIs.
Status does not request an index write; diff does not request index updates.
Snapshots contain owned Buoy strings and remain valid after closing the
repository. Native snapshots/buffers are released in `Finally` blocks even when
copying or decoding throws. Confine each repository instance to one thread or
synchronize externally. Separate operations are not an atomic snapshot of a
repository being changed concurrently by another process.

Diffs omit untracked contents, show binary-file notices, and treat individual
files larger than 8 MiB as binary. `maxBytes` stops rendering before retained patch output exceeds the limit.
It does not cap libgit2 input or diff working memory. `contextLines` accepts
0 through 1000. Large status scans/diffs can block; run them on a worker thread.
No blame/history, network transport, SHA-256 repositories (libgit2's experimental
mode is disabled), submodule recursion, or credential handling is exposed.

## Platforms and distribution

Build the bridge for the consuming application's OS and architecture. The
included CMake configuration embeds zlib, hashing and regex support and disables
network transports, SSH/HTTPS/GSSAPI/NTLM and iconv integrations. Linux deployment
uses system C runtime components; Windows builds need matching native import
libraries. A declared Buoy compiler target does not supply a native runtime slice.

## Upstream API and licensing

[libgit2's API documentation](https://libgit2.org/docs/reference/v1.9.0/) describes
the underlying repository, status and diff operations. libgit2 uses GPLv2 with its
linking exception; retained upstream `COPYING`, `AUTHORS` and component notices
are in `resources/gitkit-licenses/`. The wrapper does not add Git mutation or
network APIs. Compiler support and native backend availability are separate.

## Current native package inputs

The included runtime directories contain x86_64 Linux, ARM64 macOS and x86_64
Windows slices. Other deployment targets require their own runtime build.
See [runtime requirements](../runtime/README.md).
