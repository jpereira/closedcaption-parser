# Contribuindo

Bem simples, não tem burocracia.

## Rodando local

```
make all example   # compila a lib e o exemplo
make test          # compara com as amostras + testes da API
make test-sanitize # mesmos testes com ASan + UBSan
make lint          # cppcheck
make fuzz          # libFuzzer (precisa de clang)
make docs          # Doxygen em build/docs/html
```

## Antes de abrir o PR

- Roda `make test` e `make test-sanitize`.
- Mexeu no comportamento? Põe uma linha em `Unreleased` no `CHANGELOG.md`.
- Código no estilo que já tem (GNU, espaço antes do parênteses) e comentários em pt_BR.
- Commits curtos e diretos, até uns 100 colunas por linha.

## Release (pro mantenedor)

```
./bump.sh          # fecha a versão do VERSION (commit + tag) e abre a próxima
git push && git push origin vX.Y.Z
```

A tag dispara o workflow de release (tarball + notas do CHANGELOG) e a imagem no GitHub Packages.
