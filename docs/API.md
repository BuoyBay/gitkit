# GitKit · 0.1.0

Read-only local Git inspection for an IDE's source-control sidebar and diff view.
`Import gitkit` exposes managed Buoy objects backed by embedded libgit2 1.9.7;
consumer applications do not invoke a shell or require the Git CLI.

## Build and use

Requires Buoy 0.72+, Python 3.9+, CMake 3.16+, and a C11 compiler. The first build
downloads the pinned official libgit2 source archive and verifies its SHA256;
subsequent builds reuse the verified archive in `.buoy/gitkit`.

```sh
python3 tools/build_gitkit.py
```

The builder emits `packages/gitkit-0.1.0.buipkg` and a copy in `dist/`. Copy the
archive into a consumer project's `packages/` directory. See the complete
[runnable example](../examples/gitkit_demo.bui); its first command-line argument
is a repository directory. Objects expose the following API:

| Object/member | Behavior |
| --- | --- |
| `GitRepository(directory, discover=True)` | Open a local repository, optionally searching parents; failures throw |
| `RepositoryPath`, `WorkingDirectory`, `IsBare` | Git directory, worktree directory (empty for bare), repository kind |
| `Head()` | Owned `GitHead` snapshot with `BranchName`, `ObjectID`, `IsUnborn`, `IsDetached` |
| `Status()` | Owned array of `GitStatusEntry` snapshots; includes untracked files recursively, excludes ignored files |
| `DiffBytes(staged=False, contextLines=3, maxBytes=33554432)` | Raw unified patch bytes, comparing index/worktree or HEAD/index |
| `DiffText(...)` | Same patch decoded as UTF-8; throws `EncodingException` for non-UTF-8 patch content |
| `Close()` | Release native repository; idempotent and also called automatically by ARC |

`GitStatusEntry.RelativePath` is the literal new/current repository-relative path;
`OldPath` retains the original path for renames. `Status` is a `GitStatus` flags
enum mirroring libgit2's index/worktree/conflict flags. Convenience properties are
`HasIndexChanges`, `HasWorktreeChanges`, `IsUntracked` and `IsConflicted`.
Paths containing spaces, Unicode and newlines retain their literal contents.
The path API expects UTF-8 filenames; arbitrary non-UTF-8 POSIX filename bytes
are not representable through these String properties.

HEAD's `ObjectID` is empty for an unborn branch. A detached HEAD has an empty
`BranchName`; an unborn branch retains its symbolic branch name. Status and
staged diff use an empty tree baseline for an actually unborn HEAD. A missing
repository or corrupt repository/index throws instead of reporting a clean
repository. An absent index is a valid empty index: tracked HEAD paths appear deleted from the index
and existing worktree paths appear untracked; staged diff shows the deletions.
Bare repositories support HEAD inspection; operations requiring a worktree can throw.

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

Run the builder on each desired OS/architecture and collect `runtime/<target>`
and Windows `link/<target>` slices before sealing. `--native-only` builds a slice;
`--seal-only` compiler-checks all six targets and seals the slices present.
`--target` selects the native target; it does not install a compiler or SDK.
macOS accepts arm64/Intel cross-builds with the installed Xcode SDK. Windows
supports a native CMake/MSVC environment or a MinGW cross compiler, for example:

```sh
python3 tools/build_gitkit.py --native-only --target x86_64-windows --cc x86_64-w64-mingw32-gcc
```

Additional CMake definitions use repeated `--cmake-arg=-DNAME=value` arguments.
`BUOY` selects the compiler. Generated runtime/link slices are ignored by Git;
the sealed archive is the distributable artifact. The archive advertises only
native slices actually present, independently of the six checked Buoy targets.
Linux linking uses only system C runtime components; zlib, hashing and regex
are embedded. SSH/HTTPS/GSSAPI/NTLM/iconv integrations are disabled.

## Verification and provenance

The test runner compiles sealed-package consumers on the host and checks the
module, test and example on all six Buoy targets. It creates disposable Git
repositories covering unborn/detached/normal HEADs, staged and unstaged changes,
untracked/ignored/deleted/renamed paths, literal Unicode/newline names, conflicts,
corrupt indexes, bare repositories, closed owners and diff limits. It compares
repository file hashes before/after every inspection to verify no writes.
Only fixture creation uses the Git CLI. Use `--skip-build` to test an existing
archive without rebuilding or resealing it.

[libgit2 API documentation](https://libgit2.org/docs/reference/v1.9.0/) documents
the underlying repository, status and diff APIs. Upstream is GPLv2 with its
linking exception and bundled third-party notices; the verified upstream
`COPYING`, `AUTHORS`, and zlib license ship under `resources/gitkit-licenses/`.
The wrapper uses unmodified libgit2 sources from the pinned archive. MinGW builds
embed the compiler support runtime and winpthreads rather than requiring extra
non-system DLLs; corresponding runtime notices are also included.

The current source package includes ARM64 macOS, x86_64 Linux and x86_64
Windows slices rebuilt from the current native source. The Windows bridge was
cross-built with MinGW and pinned libgit2; its sealed consumer was compiled and
executed natively on Windows. Intel macOS is not supplied or native-tested.

## Current native package inputs

The repository includes verified x86_64 Linux, ARM64 macOS and x86_64 Windows
native slices under `runtime/`. Other targets are not bundled or native-tested.
See [runtime scope](../runtime/README.md).

Test suites and fixtures are maintainer-local and are not part of this public repository.
