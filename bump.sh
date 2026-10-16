#!/usr/bin/env bash
#
# bump.sh - Fecha a versão atual (release) e abre a próxima, numa execução só
#
#   ... commits da 1.0.1 ── Release 1.0.1 (tag v1.0.1) ── Bump para 1.0.2 ── ...
#
# A versão atual é a do arquivo VERSION (em desenvolvimento desde o último bump).
#
#   1. Release 1.0.1 (pula se a tag v1.0.1 já existe)
#      - CHANGELOG.md: "Unreleased" vira "## [1.0.1] - data" e abre um Unreleased novo.
#        (se já tiver a seção da versão, só mantém)
#      - Commit "Release 1.0.1" com a tag anotada v1.0.1.
#   2. Bump para 1.0.2 (ou 1.1.0 com --minor, 2.0.0 com --major, ou a versão
#      informada, ex.: ./bump.sh 1.5.0)
#      - VERSION e os CLOSEDCAPTION_VERSION_* do include/closedcaption/caption-version.h
#      - Commit "Bump para 1.0.2" (sem tag).
#
# Uso: ./bump.sh [-n|--dry-run] [--minor|--major|X.Y.Z]
#
# O push fica por sua conta: git push && git push origin vX.Y.Z
#
set -euo pipefail
cd "$(dirname "$0")"

uso() { echo "Uso: $0 [-n|--dry-run] [--minor|--major|X.Y.Z]" >&2; exit 1; }

DRY_RUN=0; versao_pedida=""; parte=""
for arg in "$@"; do
    case "$arg" in
        -n|--dry-run) DRY_RUN=1 ;;
        --minor|--major)
            [ -z "$parte" ] || { echo "❌ Use --minor ou --major, não os dois." >&2; uso; }
            parte="${arg#--}" ;;
        -h|--help) uso ;;
        -*) echo "❌ Opção desconhecida: $arg" >&2; uso ;;
        *)
            [ -z "$versao_pedida" ] || { echo "❌ Informe uma versão só." >&2; uso; }
            versao_pedida="$arg" ;;
    esac
done
if [ -n "$versao_pedida" ] && [ -n "$parte" ]; then
    echo "❌ Use --${parte} ou a versão X.Y.Z, não os dois." >&2; uso
fi
if [ -n "$versao_pedida" ] && ! [[ "$versao_pedida" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    echo "❌ Versão '$versao_pedida' fora do formato X.Y.Z." >&2; exit 1
fi
if [ "$DRY_RUN" -eq 0 ] && [ -n "$(git status --porcelain)" ]; then
    echo "❌ Há alterações não commitadas. Commite ou descarte antes do bump." >&2; exit 1
fi

atual="$(tr -d '[:space:]' < VERSION)"
if ! [[ "$atual" =~ ^([0-9]+)\.([0-9]+)\.([0-9]+)$ ]]; then
    echo "❌ Versão '$atual' do VERSION fora do formato X.Y.Z." >&2; exit 1
fi
x="${BASH_REMATCH[1]}" y="${BASH_REMATCH[2]}" z="${BASH_REMATCH[3]}"
case "$parte" in
    major) proxima="$((x + 1)).0.0" ;;
    minor) proxima="${x}.$((y + 1)).0" ;;
    *) proxima="${x}.${y}.$((z + 1))" ;;
esac
nova="${versao_pedida:-$proxima}"

# a > b, comparando X, Y e Z como números (1.10.0 > 1.9.0)
maior() {
    [ "$1" != "$2" ] &&
        [ "$(printf '%s\n%s\n' "$1" "$2" | sort -t. -k1,1n -k2,2n -k3,3n | tail -n1)" = "$1" ]
}
maior "$nova" "$atual" || { echo "❌ A versão ${nova} não é maior que a atual (${atual})." >&2; exit 1; }

fazer_release=1
git rev-parse -q --verify "refs/tags/v${atual}" >/dev/null && fazer_release=0

if [ "$DRY_RUN" -eq 1 ]; then
    [ "$fazer_release" -eq 1 ] && echo "🔖 Release ${atual}: commit \"Release ${atual}\" + tag v${atual}" \
                               || echo "ℹ️ A v${atual} já existe: só o bump."
    echo "🚀 Bump para ${nova}: VERSION e include/closedcaption/caption-version.h"
    exit 0
fi

# A mensagem do commit respeita o limite de 100 colunas
commitar() {
    local msg; msg="$(mktemp "${TMPDIR:-/tmp}/cc-commit.XXXXXX")"
    printf '%s\n' "$1" > "$msg"
    awk 'length > 100 { print; erro = 1 } END { exit erro }' "$msg"
    git commit -a -F "$msg"
    rm -f "$msg"
}

if [ "$fazer_release" -eq 1 ]; then
    echo "🔖 Release ${atual}"
    if ! grep -q "^## \[${atual}\]" CHANGELOG.md; then
        hoje="$(date +%Y-%m-%d)"
        tmp="$(mktemp)"
        awk -v v="$atual" -v d="$hoje" '
            /^## \[Unreleased\]/ && !feito { print; print ""; print "## [" v "] - " d; feito = 1; next }
            { print }' CHANGELOG.md > "$tmp" && mv "$tmp" CHANGELOG.md
        commitar "Release ${atual}"
    else
        echo "ℹ️ CHANGELOG já tem a ${atual}: a tag vai no commit atual."
    fi
    git tag -a "v${atual}" -m "Release ${atual}"
else
    echo "ℹ️ A v${atual} já existe: só o bump."
fi

echo "🚀 Bump para ${nova} (em desenvolvimento)"
IFS=. read -r nx ny nz <<< "$nova"
echo "$nova" > VERSION
sed -i.bak -E \
    -e "s/(CLOSEDCAPTION_VERSION_MAJOR) [0-9]+/\1 ${nx}/" \
    -e "s/(CLOSEDCAPTION_VERSION_MINOR) [0-9]+/\1 ${ny}/" \
    -e "s/(CLOSEDCAPTION_VERSION_PATCH) [0-9]+/\1 ${nz}/" include/closedcaption/caption-version.h
rm -f include/closedcaption/caption-version.h.bak
git --no-pager diff --stat
commitar "Bump para ${nova}"
echo "✅ Pronto. Para publicar: git push && git push origin v${atual}"
