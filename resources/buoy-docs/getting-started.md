---
title: "GitKit getting started"
summary: "Install and use GitKit with explicit configuration and resource ownership."
category: "Guides"
order: 10
keywords: ["git", "status", "diff", "repository", "libgit2"]
related: ["index.md", "api.md", "typed-api.md"]
---

# GitKit getting started

Copy the gitkit archive into packages and retain the matching native runtime payload when distributing the built app. The example opens the current directory, searching parents, prints HEAD and status, and always closes the owner. It performs no repository write.

```basic
Import gitkit

Sub Main()
    Dim repository As New GitRepository(".")
    Try
        Dim head As GitHead = repository.Head()
        Print head.BranchName
        For Each entry As GitStatusEntry In repository.Status()
            Print entry.RelativePath
        Next
    Finally
        repository.Close()
    End Try
End Sub
```

## HEAD and status

An unborn branch has an empty ObjectID but retains its symbolic branch name. A detached HEAD has an empty BranchName. Status includes untracked files recursively and excludes ignored files. Missing repositories and corrupt repository/index state throw rather than pretending the working tree is clean. An absent index is valid and treated as empty: tracked HEAD paths appear deleted from the index, existing worktree paths appear untracked, and staged diff shows the deletions.

## Diffs

DiffBytes preserves arbitrary patch bytes. DiffText decodes UTF-8 and throws EncodingException if decoding fails. Untracked contents are not diffed; binary files produce notices and files over 8 MiB are treated as binary. maxBytes stops rendering before retained patch output exceeds the limit; it does not cap libgit2 input or diff working memory. Bare repositories allow HEAD inspection but worktree operations may fail.

## Read next

- [Overview](index.md)
- [Getting started](getting-started.md)
- [API reference](api.md)
