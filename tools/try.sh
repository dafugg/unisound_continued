#!/bin/sh
# Compile the .C in DIR with Turbo C++ 3.0 (C:\TC\BIN\TCC -c -mt -O -d; override the flags with
# $CCFLAGS, or set CC=BCC for Borland C++ 3.1) inside this agent's
# 86Box VM ($UNIVM), then compare the OBJ with
# UNISOUND.COM from START on.   Usage: try.sh DIR START [-v FUNC_ADDR]
# Exactly one .C file per DIR. Compiler output is in DIR/out/LOG.TXT, the OBJ in DIR/out/.
set -e
. "$(dirname "$0")/env.sh"
dir=$(cd "$1" && pwd); start=$2; shift 2
c=$(cd "$dir" && ls *.C *.c 2>/dev/null | head -1)
[ -n "$c" ] || { echo "no .C file in $dir"; exit 2; }
base=${c%.*}
case "${CC:-TCC}" in
  BCC) cc='C:\BC\BIN\BCC' ;;
  *)   cc='C:\TC\BIN\TCC' ;;
esac
printf '%s -c -mt %s %s > LOG.TXT\r\n' "$cc" "${CCFLAGS:--O -d}" "$c" > "$dir/JOB.BAT"
"$BOXPY" -I "$ROOT/tools/dosbuild.py" "$dir" 300 > "$dir/screen.txt" 2>&1 || { cat "$dir/screen.txt"; exit 1; }
grep -v -e '^Borland C++' -e '^Turbo C++' -e 'Available memory' -e '^ *$' "$dir/out/LOG.TXT" || true
obj=$(ls "$dir/out/" | grep -i "^$base.obj$" || true)
[ -n "$obj" ] || { echo "no OBJ produced"; exit 1; }
"$PYTHON" -I "$ROOT/tools/objmatch.py" "$dir/out/$obj" "$start" "$@"
