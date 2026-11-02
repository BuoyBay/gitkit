# Inspect local Git status and diffs with embedded libgit2

Read-only local Git inspection for an IDE's source-control sidebar and diff view.
`Import gitkit` exposes managed objects backed by embedded libgit2 1.9.7;
consumer applications do not invoke a shell or require the Git CLI.

## Install

From a Buoy consumer project with BPM installed:

```sh
bpm add gitkit --git https://github.com/BuoyBay/gitkit.git --rev refs/heads/main
```

BPM records the resolved commit in its lockfile. Keep that lockfile with the
consumer project for reproducible installs.

## Example

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

## Build and API reference

Source is in `modules/`, with native/runtime inputs where required. Use the
[package guide](docs/API.md) for backend-specific build prerequisites, ownership
rules and supported targets. Rebuilding a native backend requires that target's
compiler and SDK; sealing an archive alone does not rebuild the backend.

## Local Buoy setup

On your own machine, install Buoy and run:

```sh
buoy ai init --both
```


## Contributing

[Contributing](CONTRIBUTING.md) · [License](LICENSE)
