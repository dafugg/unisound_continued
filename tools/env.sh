# Sourced by the shell tools. Settings come from local.env in the repo root (not committed);
# see local.env.example.
ROOT=$(cd "$(dirname "$0")/.." && pwd)
[ -f "$ROOT/local.env" ] && . "$ROOT/local.env"
: "${BOX86MCP:?set BOX86MCP to an 86box-mcp checkout (see local.env.example)}"
: "${UNIVM:?set UNIVM to the DOS VM directory (see local.env.example)}"
BOXPY=${BOXPY:-$BOX86MCP/server/.venv/bin/python}
PYTHON=${PYTHON:-python3}
export UNIVM BOX86MCP MTOOLS_DIR UNISOUND_COM BOX86_ROMS
