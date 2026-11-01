---
title: "gitkit typed API"
summary: "Source-derived public signatures, properties, parameters, defaults, and return values."
category: "Reference"
order: 21
types: ["GitHead", "GitRepository", "GitStatus", "GitStatusEntry"]
dependencies: []
symbols: ["GitHead", "GitRepository", "GitStatus", "GitStatusEntry"]
related: ["api.md", "getting-started.md", "index.md"]
---

# gitkit typed API

Generated from the package source declarations. Platform guards are shown where present; inherited members are documented on the base type. See the [API contracts](api.md) for behavior and limitations.

## GitStatus

`Public Flags Enum GitStatus As Integer`

### Values

| Name | Type | Value / Default | Description |
| --- | --- | --- | --- |
| `Current` | `GitStatus` | `0` | — |
| `IndexNew` | `GitStatus` | `1` | — |
| `IndexModified` | `GitStatus` | `2` | — |
| `IndexDeleted` | `GitStatus` | `4` | — |
| `IndexRenamed` | `GitStatus` | `8` | — |
| `IndexTypeChange` | `GitStatus` | `16` | — |
| `WorktreeNew` | `GitStatus` | `128` | — |
| `WorktreeModified` | `GitStatus` | `256` | — |
| `WorktreeDeleted` | `GitStatus` | `512` | — |
| `WorktreeTypeChange` | `GitStatus` | `1024` | — |
| `WorktreeRenamed` | `GitStatus` | `2048` | — |
| `WorktreeUnreadable` | `GitStatus` | `4096` | — |
| `Ignored` | `GitStatus` | `16384` | — |
| `Conflicted` | `GitStatus` | `32768` | — |

## GitHead

`Public Class GitHead`

An owned value snapshot: subsequent repository changes do not mutate this object.

### Properties

| Name | Type | Read-Only | Shared | Default | Description |
| --- | --- | --- | --- | --- | --- |
| `BranchName` | `String` | No | No | — | — |
| `ObjectID` | `String` | No | No | — | — |
| `IsUnborn` | `Boolean` | No | No | — | — |
| `IsDetached` | `Boolean` | No | No | — | — |

## GitStatusEntry

`Public Class GitStatusEntry`

Paths are literal UTF-8 repository-relative paths, never shell-quoted porcelain.

### Properties

| Name | Type | Read-Only | Shared | Default | Description |
| --- | --- | --- | --- | --- | --- |
| `RelativePath` | `String` | No | No | — | — |
| `OldPath` | `String` | No | No | — | — |
| `Status` | `GitStatus` | No | No | — | — |
| `HasIndexChanges` | `Boolean` | Yes | No | — | — |
| `HasWorktreeChanges` | `Boolean` | Yes | No | — | — |
| `IsUntracked` | `Boolean` | Yes | No | — | — |
| `IsConflicted` | `Boolean` | Yes | No | — | — |

## GitRepository

`Public Class GitRepository`

A local, read-only repository owner. Confine each instance to one thread.

### Properties

| Name | Type | Read-Only | Shared | Default | Description |
| --- | --- | --- | --- | --- | --- |
| `RepositoryPath` | `String` | Yes | No | — | — |
| `WorkingDirectory` | `String` | Yes | No | — | — |
| `IsBare` | `Boolean` | Yes | No | — | — |

### Constructor

`Public Sub Constructor(directory As String, discover As Boolean = True)`

discover=True searches parent directories; False opens only the supplied location.

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `directory` | `String` | `ByVal` | Required |
| `discover` | `Boolean` | `ByVal` | `True` |

Returns: no value.

### Close

`Public Sub Close()`

No parameters.

Returns: no value.

### Destructor

`Public Sub Destructor()`

No parameters.

Returns: no value.

### Head

`Public Function Head() As GitHead`

No parameters.

#### Returns

| Type |
| --- |
| `GitHead` |

### Status

`Public Function Status() As Array Of GitStatusEntry`

No parameters.

#### Returns

| Type |
| --- |
| `Array Of GitStatusEntry` |

### DiffBytes

`Public Function DiffBytes(staged As Boolean = False, contextLines As Integer = 3, maxBytes As Integer = 33554432) As Bytes`

Diff excludes untracked file contents; use Status to list untracked paths.

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `staged` | `Boolean` | `ByVal` | `False` |
| `contextLines` | `Integer` | `ByVal` | `3` |
| `maxBytes` | `Integer` | `ByVal` | `33554432` |

#### Returns

| Type |
| --- |
| `Bytes` |

### DiffText

`Public Function DiffText(staged As Boolean = False, contextLines As Integer = 3, maxBytes As Integer = 33554432) As String`

Throws EncodingException when a patch contains non-UTF-8 file text.

#### Parameters

| Name | Type | Passing | Default |
| --- | --- | --- | --- |
| `staged` | `Boolean` | `ByVal` | `False` |
| `contextLines` | `Integer` | `ByVal` | `3` |
| `maxBytes` | `Integer` | `ByVal` | `33554432` |

#### Returns

| Type |
| --- |
| `String` |
