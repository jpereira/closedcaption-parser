# Changelog

All the changes in the project. Format based on [Keep a Changelog](https://keepachangelog.com/),
versions follow [SemVer](https://semver.org/).

## [Unreleased]

### Added
- Parsers for the DRCS (1 and 2 bytes), bitmap, geometric graphics and synthesized sound data
  units, and `caption_superimpose_new()` (text of the superimpose, one message per data unit)
- `caption_management_load_unit()` to walk through all the data units of a data group
- `--units` in the example (and `make run-example-units`) to show those data units
- `tests/sintetico-unidades.ts`: synthetic stream with one data unit of each type, and tests
  in `tests/test-api.c`

### Changed
- `caption_superimpose_new()` replaces the global hooks while it runs and restores them after

## [1.0.1] - 2026-10-16

First "real" release of the lib.

### Added
- `VERSION` file, `closedcaption_version()` and `CLOSEDCAPTION_VERSION_*` in `caption-version.h`
- `make install` / `make uninstall` (`PREFIX` and `DESTDIR`), `.a` lib, soname
  (`libclosedcaption.so.1`) and `libclosedcaption.pc` for pkg-config
- Headers moved to `include/closedcaption/`, use `#include <closedcaption/caption.h>`
- `make dist` (release tarball), `make lint` (cppcheck), `make test-sanitize` (ASan + UBSan)
  and `make fuzz` (libFuzzer)
- `tests/test-api.c`: version, missing file and broken or garbage TS/PES
- `bump.sh` (release + bump), matrix CI (Linux gcc/clang, macOS), release workflow,
  Dependabot, `LICENSE` (LGPL-3.0), `CONTRIBUTING.md` and `SECURITY.md`
- Container image on GitHub Packages (`ghcr.io/jpereira/libclosedcaption`)
- English (default) and pt_BR docs: README, API docs (Doxygen), samples, CONTRIBUTING and SECURITY

### Changed
- Project renamed from `closedcaption-parser` to `libclosedcaption`
- Debug macros `ATV_DEBUG` and `ATV_HEXDUMP` renamed to `DTV_DEBUG` and `DTV_HEXDUMP`
