# Changelog

Tudo que mudou no projeto. Formato baseado no [Keep a Changelog](https://keepachangelog.com/),
versões seguem o [SemVer](https://semver.org/).

## [Unreleased]

## [1.0.1] - 2026-10-16

Primeira release "de verdade" da lib.

### Added
- Arquivo `VERSION`, `closedcaption_version()` e `CLOSEDCAPTION_VERSION_*` no `caption-version.h`
- `make install` / `make uninstall` (`PREFIX` e `DESTDIR`), lib `.a`, soname (`libclosedcaption.so.1`)
  e `libclosedcaption.pc` pro pkg-config
- `make dist` (tarball da release), `make lint` (cppcheck), `make test-sanitize` (ASan + UBSan)
  e `make fuzz` (libFuzzer)
- `tests/test-api.c`: versão, arquivo inexistente e TS/PES quebrado ou com lixo
- `bump.sh` (release + bump), CI em matrix (Linux gcc/clang, macOS), workflow de release,
  Dependabot, `LICENSE` (LGPL-3.0), `CONTRIBUTING.md` e `SECURITY.md`
- Imagem de container no GitHub Packages (`ghcr.io/jpereira/libclosedcaption`)

### Changed
- Projeto renomeado de `closedcaption-parser` para `libclosedcaption`
