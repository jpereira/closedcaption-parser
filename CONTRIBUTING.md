🇺🇸 **English** · 🇧🇷 [Português (pt_BR)](CONTRIBUTING.pt_BR.md)

# Contributing

Pretty simple, no bureaucracy.

## Running locally

```
make all example   # builds the lib and the example
make test          # compares with the samples + API tests
make test-sanitize # same tests with ASan + UBSan
make lint          # cppcheck
make fuzz          # libFuzzer (needs clang)
make docs          # Doxygen in build/docs/html (en) and build/docs/html/pt_BR
make check-i18n    # checks that the pt_BR header copies still match include/
```

## Before opening the PR

- Run `make test` and `make test-sanitize`.
- Changed behavior? Add one line under `Unreleased` in `CHANGELOG.md`.
- Changed a public header? Update the doc comments in `include/` (English) and the pt_BR copy
  in `docs/pt_BR/closedcaption/` too (just the comments, `make check-i18n` complains if the
  declarations differ).
- Code in the style that's already there (GNU, space before the parenthesis). Comments in
  `src/` are in pt_BR, the public API docs are in both.
- Short and direct commits, up to ~100 columns per line.

## Release (maintainer only)

```
./bump.sh          # closes the VERSION version (commit + tag) and opens the next one
git push && git push origin vX.Y.Z
```

The tag fires the release workflow (tarball + CHANGELOG notes) and the image on GitHub Packages.
