REM Rebuild UNISOUND.COM with Turbo C++ 3.0 (byte-identical to v0.82a)
TCC -c -mt -O -d UNISOUND.C
TLINK /c /t C:\TC\LIB\C0T UNISOUND, UNISOUND, , C:\TC\LIB\CS
