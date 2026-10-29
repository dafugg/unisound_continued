#!/bin/sh
# Fetch and build what CI's FreeDOS build and smoke test need into DEST: 86Box with the automation
# socket (DEST/bin/86Box), its ROM set (DEST/roms), a Python with 86box-mcp's box86mcp (DEST/venv)
# and the FreeDOS 1.4 LiteUSB image (DEST/FD14LITE.img). Parts already in DEST are kept, so CI
# caches DEST under a key made from this file. Needs git, curl, unzip, python3-venv and 86Box's
# build packages (see .github/actions/setup-86box/action.yml).
# Usage: tools/86box-deps.sh DEST
set -eu
BOX86_SHA=8215e486ca6b93a71040a0a28a6c94fd3be1daae   # 86Box/86Box master, 2026-10-29
ROMS_SHA=6122ea433b5d69726844d5748ef930cdf6ad8f48    # 86Box/roms master, 2026-10-29
MCP_SHA=534fed4ae51358034e72fa68981357e895f43a3e     # dafugg/86box-mcp main, 2026-10-29
FD_URL=https://www.ibiblio.org/pub/micro/pc-stuff/freedos/files/distributions/1.4/FD14-LiteUSB.zip
FD_SHA256=857dcd2ebf9d3d094320154db5fb5b830acba6fb98f981a95a0ca7ab3350338b
D=$1
mkdir -p "$D"

fetch() {  # fetch REPO SHA DIR: one commit of a GitHub repo, no history
    git init -q "$3"
    git -C "$3" fetch -q --depth 1 "https://github.com/$1.git" "$2"
    git -C "$3" checkout -q FETCH_HEAD
}

if [ ! -x "$D/bin/86Box" ]; then
    fetch 86Box/86Box "$BOX86_SHA" "$D/86box-src"
    cmake -G Ninja -S "$D/86box-src" -B "$D/86box-src/build" -D CMAKE_BUILD_TYPE=Release -D QT=ON \
        -D USE_QT6=ON -D AUTOMATION=ON -D DISCORD=OFF -D MUNT=OFF -D SOUNDCANVAS=OFF
    cmake --build "$D/86box-src/build"
    mkdir -p "$D/bin"
    cp "$D/86box-src/build/src/86Box" "$D/bin/86Box"
    rm -rf -- "${D:?}/86box-src"
fi
[ -d "$D/roms" ] || fetch 86Box/roms "$ROMS_SHA" "$D/roms"
if [ ! -x "$D/venv/bin/python" ]; then
    fetch dafugg/86box-mcp "$MCP_SHA" "$D/86box-mcp"
    python3 -m venv "$D/venv"
    "$D/venv/bin/pip" install -q "$D/86box-mcp/server"
fi
if [ ! -f "$D/FD14LITE.img" ]; then
    curl -sSfL -o "$D/FD14-LiteUSB.zip" "$FD_URL"
    echo "$FD_SHA256  $D/FD14-LiteUSB.zip" | sha256sum -c -
    unzip -q -o "$D/FD14-LiteUSB.zip" FD14LITE.img -d "$D"
    rm -f -- "$D/FD14-LiteUSB.zip"
fi
