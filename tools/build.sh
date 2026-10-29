#!/bin/sh
# Build src/UNISOUND.C with BUILD.BAT inside the 86Box VM and check the result against
# UNISOUND.SHA256. Output lands in build/out/ (UNISOUND.COM, UNISOUND.OBJ, LOG.TXT).
# If the original UNISOUND.COM is available (orig/ or $UNISOUND_COM), differences are listed.
# --no-hash: only check that it compiles and links (for changes that alter the program).
set -e
nohash=; [ "$1" = --no-hash ] && nohash=1
. "$(dirname "$0")/env.sh"
B=$ROOT/build
rm -rf "$B"; mkdir -p "$B"
cp "$ROOT/src/UNISOUND.C" "$ROOT/BUILD.BAT" "$B/"
printf 'SET PATH=C:\\TC\\BIN;%%PATH%%\r\n%%COMSPEC%% /C BUILD.BAT > LOG.TXT\r\n' > "$B/JOB.BAT"
"$BOXPY" -I "$ROOT/tools/dosbuild.py" "$B" 300 > "$B/screen.txt" 2>&1 || { cat "$B/screen.txt"; exit 1; }
tr -d '\r' < "$B/out/LOG.TXT" | grep -i -e error -e fatal -e undefined -e warning || true
ls "$B/out" | grep -qi '^unisound\.obj$' || { echo "compile failed; see $B/out/LOG.TXT"; exit 1; }
com=$(ls "$B/out" | grep -i '^unisound\.com$' || true)
[ -n "$com" ] || { echo "no UNISOUND.COM produced; see $B/out/LOG.TXT"; exit 1; }
[ "$com" = UNISOUND.COM ] || mv "$B/out/$com" "$B/out/UNISOUND.COM"
if [ -n "$nohash" ]; then
    echo "built UNISOUND.COM ($(wc -c < "$B/out/UNISOUND.COM") bytes), hash not checked"
elif (cd "$B/out" && sha256sum -c "$ROOT/UNISOUND.SHA256"); then
    echo "IDENTICAL to UNISOUND.COM v0.82a"
else
    orig=${UNISOUND_COM:-$ROOT/orig/UNISOUND.COM}
    [ -f "$orig" ] && "$PYTHON" -I "$ROOT/tools/cmpcom.py" "$B/out/UNISOUND.COM"
    exit 1
fi
