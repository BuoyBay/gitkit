# Native GitKit runtimes

The supplied bridges target x86_64 Linux, ARM64 macOS and x86_64 Windows. They use
libgit2 1.9.7 and this package's native bridge; third-party notices are retained
under `resources/gitkit-licenses/`. The Windows import library is under
`link/x86_64-pc-windows-msvc/`.

Compile a matching bridge for any other deployment target. The included CMake
project accepts the libgit2 source directory through `LIBGIT2_SOURCE`. Keep
runtime and import-library architecture aligned with the consuming application.
Compiler checks do not provide missing runtime binaries.

See the [API and build guide](../docs/API.md).
