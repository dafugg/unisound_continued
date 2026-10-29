#!/bin/sh
# Build src/UNISOUND.C with BUILD.BAT in DOSBox-X (used by CI; tools/build.sh uses the 86Box VM)
# and check the result against UNISOUND.SHA256. Output lands in build/c/WORK/.
# Usage: tools/dosbox-build.sh TCDIR [--no-hash]
# TCDIR is a Turbo C++ 3.0 install (BIN/, LIB/, INCLUDE/); it is copied to C:\TC.
# --no-hash: only check that it compiles and links (for changes that alter the program).
set -e
ROOT=$(cd "$(dirname "$0")/.." && pwd)
TC=${1:?usage: tools/dosbox-build.sh TCDIR [--no-hash]}
nohash=; [ "$2" = --no-hash ] && nohash=1
B=$ROOT/build
rm -rf "$B"; mkdir -p "$B/c/WORK"
cp -R "$TC" "$B/c/TC"
cp "$ROOT/src/UNISOUND.C" "$ROOT/BUILD.BAT" "$B/c/WORK/"
printf 'SET PATH=C:\\TC\\BIN;%%PATH%%\r\n%%COMSPEC%% /C BUILD.BAT > LOG.TXT\r\n' > "$B/c/WORK/JOB.BAT"
SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy timeout 300 dosbox-x -nopromptfolder -fastlaunch \
    -silent -exit -set "dosbox memsize=16" -c "MOUNT C $B/c" -c "C:" -c "CD \\WORK" \
    -c "CALL JOB.BAT" > "$B/dosbox.txt" 2>&1 || { cat "$B/dosbox.txt"; exit 1; }
W=$B/c/WORK
[ -f "$W/LOG.TXT" ] || { cat "$B/dosbox.txt"; echo "JOB.BAT did not run"; exit 1; }
tr -d '\r' < "$W/LOG.TXT"
[ -f "$W/UNISOUND.OBJ" ] || { echo "compile failed"; exit 1; }
[ -f "$W/UNISOUND.COM" ] || { echo "no UNISOUND.COM produced"; exit 1; }
if [ -n "$nohash" ]; then
    echo "built UNISOUND.COM ($(wc -c < "$W/UNISOUND.COM") bytes), hash not checked"
elif (cd "$W" && sha256sum -c "$ROOT/UNISOUND.SHA256"); then
    echo "IDENTICAL to UNISOUND.COM v0.82a"
else
    exit 1
fi
