---
title: "GitKit API"
summary: "Public types, operations, error behavior, and limits for GitKit."
category: "Reference"
order: 20
keywords: ["git", "status", "diff", "repository", "libgit2"]
symbols: ["GitRepository", "GitHead", "GitStatusEntry", "GitStatus", "GitRepository.Head", "GitRepository.Status", "GitRepository.DiffBytes", "GitRepository.Close"]
related: ["index.md", "getting-started.md", "typed-api.md"]
---

# GitKit API

| API | Contract |
| --- | --- |
| GitRepository(directory, discover) | Open a local repository; discover defaults to True and searches parents. |
| RepositoryPath, WorkingDirectory, IsBare | Git directory, working tree directory, and repository kind. |
| Head() | GitHead with BranchName, ObjectID, IsUnborn, IsDetached. |
| Status() | Array Of GitStatusEntry snapshots. |
| DiffBytes(staged, contextLines, maxBytes) | Bytes for index/worktree or HEAD/index; defaults False, 3, 33554432. |
| DiffText(staged, contextLines, maxBytes) | Same diff as UTF-8 String; contextLines accepts 0 through 1000. |
| Close() | Release the native owner; safe to repeat. |
| GitStatusEntry | RelativePath, OldPath, Status, HasIndexChanges, HasWorktreeChanges, IsUntracked, IsConflicted. |
| GitStatus | Flags describing index, worktree, and conflict state. |

## Paths and failure handling

RelativePath is the current/new literal repository-relative path; OldPath holds the previous rename path. Spaces, Unicode, and newlines are preserved. String paths require UTF-8 filenames, so arbitrary POSIX filename bytes are not representable.

Native operation failures throw. Empty constructor paths, NUL bytes, closed owners, invalid diff parameters, and oversized results reject rather than producing partial success. Keep snapshot data if it is needed after closing the repository.

## Read next

- [Overview](index.md)
- [Getting started](getting-started.md)
- [API reference](api.md)

[Full typed API: properties, parameters, defaults, and returns](typed-api.md)
