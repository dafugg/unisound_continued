REM Build UNISOUND.COM with Turbo C++ 3.0: a medium-model EXE (MZ header) under the .COM name.
REM DOS loads it by the header, so existing UNISOUND.COM command lines keep working.
TCC -c -mm -O -d UNISOUND.C
TLINK /c C:\TC\LIB\C0M UNISOUND, UNISOUND, , C:\TC\LIB\CM
IF EXIST UNISOUND.COM DEL UNISOUND.COM
REN UNISOUND.EXE UNISOUND.COM
