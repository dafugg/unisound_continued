# Notes on the UNISOUND 0.82a rebuild

## Toolchain
- **Compiler: Turbo C++ 3.0 TCC.EXE** (Feb 1992), `-c -mt -O -d`.
  - Borland C++ 3.1 BCC is very close. It pushes a `long 0` argument as
    `xor ax,ax / xor dx,dx / push ax / push dx`; TC 3.0 emits `xor ax,ax / push ax / push ax`.
  - Borland C++ 3.0 BCC orders the arguments to `N_SCOPY@` (initialized local-array copies)
    differently.
  - `-d` is required. It merges duplicate strings, which enables cross-jumping of identical code
    tails (seen in configure_pnp and init_nonpnp_sb16), and it pools the literals at the end of the module's data.
- **Startup and runtime: TC++ 3.0 / BC++ 3.0 C0T.OBJ and CS.LIB**, not 3.1's.
  - 3.1's C0T differs (617 vs 666 bytes).
  - 3.1's IOERROR maps 0x30 DOS errors (3.0: 0x23).
  - VPRINTER, PUTC and WRITE differ slightly in 3.1.
- Link: `TLINK /c /t C0T UNISOUND, UNISOUND, , CS` (TLINK 5.0 from TC++ 3.0).

## Memory map (offsets in the .COM, loaded at 0x100)

| Range | Contents |
|---|---|
| 0x0100–0x0368 | C0T startup |
| 0x0369–0x9184 | Author code; `main` is the first function. One module, compiled from one source file |
| 0x9185–0xb354 | CS.LIB runtime modules; 0xb355–0xb35f is padding to the DGROUP paragraph |
| 0xb360–0xb3f1 | C0T `_DATA` |
| 0xb3f2–0xc1fd | Initialized globals, in definition order: EMU8000 init tables, the CS4232 firmware blob (`g_b4f2`, `g_b4fa`), port/IRQ/DMA/volume settings |
| 0xc1fe–0xc5b6 | Templates for initialized local arrays, plus a few globals, in function order |
| 0xc5b7– | The `-d` string-literal pool, then runtime-library data (`_ctype` at 0xd370, …) |

## Quirks needed for an exact match
- The banner literal ends in an explicit `"\0"`. With `-d`, every `""` in the program merges
  into its tail.
- `opl_write` has no prototype. Callers widen unsigned char arguments with `mov ah,0`, which they
  don't do for prototyped functions.
- `inp`, `outp` and `outpw` are called as library functions: they are `#undef`'d after the
  includes. `inpw` stays the intrinsic macro.
- Switch statements compile to jump tables in the code segment right after the function
  (`jmp cs:[bx+T]`).

## How it was done
The program was split into 8 address ranges. Each range was decompiled separately: compiled in an
86Box DOS VM and compared function by function with fixups masked (`tools/try.sh`, `objmatch.py`).
The 8 sources were then merged into one module. The data section was generated from the binary,
and the link was iterated until the whole file was identical.

At first, where the parts disagreed on a global's type, the merged source used `#define` type-view
macros. It turned out none of them were needed under TCC 3.0. The same was true of a few register
allocation workarounds the parts had picked up while being compiled with BC++ 3.1.
