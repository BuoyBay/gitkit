# Native GitKit runtimes

The x86_64 Linux and ARM64 macOS libraries were built from this package's native
bridge and SHA-256 verified libgit2 1.9.7 source. Each was sealed from the package
source and tested against disposable repositories, including dirty, detached,
conflicted, corrupt, bare and unborn states. Retained third-party notices are
under `resources/gitkit-licenses/`.

Other target slices are not supplied or native-tested in this repository. Build
and validate them before use; compiler checks alone do not establish runtime
support. The platform build helper lives in the BuoyBay devtools repository.

The x86_64 Windows slice was rebuilt from the current bridge and pinned libgit2
1.9.7 with MinGW. A Windows-native compiled sealed consumer exercised repository
identity, status, diffs, Unicode names, error handling and closure; fixture files
remained unchanged. Its import library is in `link/x86_64-pc-windows-msvc/`.
