; TRACEPNP [options]: run UNISOUND.COM [options] with the trap flag set and log every
; OUT DX,AL to the ISA PnP ports (279h address, A79h write data) and to the ESS
; configuration ports 660h/661h and 800h/801h. Prints "port=value" for each, in order.
; Build: TASM TRACEPNP.ASM / TLINK /t TRACEPNP.OBJ
        .model tiny
        .code
        org 100h
start:
        mov     sp, offset stktop
        mov     ah, 4ah                 ; keep 64 KB for ourselves
        mov     bx, 1000h
        int     21h
        mov     ax, 3501h               ; save and hook INT 1
        int     21h
        mov     word ptr old1, bx
        mov     word ptr old1+2, es
        mov     ax, 2501h
        mov     dx, offset int1
        int     21h
        mov     ax, 3503h               ; and INT 3
        int     21h
        mov     word ptr old3, bx
        mov     word ptr old3+2, es
        mov     ax, 2503h
        mov     dx, offset int3
        int     21h
        mov     ax, cs                  ; parameter block: inherit env, our command tail
        mov     pb_tail+2, ax
        mov     pb_fcb1+2, ax
        mov     pb_fcb2+2, ax
        mov     es, ax
        mov     bx, offset pblock
        mov     dx, offset fname
        mov     ax, 4b01h               ; load, don't execute
        int     21h
        jnc     loaded
        mov     dx, offset msg_load
        mov     ah, 9
        int     21h
        jmp     restore
loaded:
        push    cs
        pop     ds
        mov     ah, 62h                 ; the child's PSP is now current
        int     21h
        mov     childpsp, bx
        mov     es, bx                  ; child returns to child_done
        mov     word ptr es:[0ah], offset child_done
        mov     word ptr es:[0ch], cs
        mov     savesp, sp
        cli
        mov     ss, word ptr pb_sssp+2
        mov     sp, word ptr pb_sssp
        sti
        pushf
        pop     ax
        or      ax, 100h                ; TF
        push    ax
        push    word ptr pb_csip+2
        push    word ptr pb_csip
        mov     ax, childpsp
        mov     ds, ax
        mov     es, ax
        xor     ax, ax
        iret
child_done:
        pushf                           ; stop single-stepping
        pop     ax
        and     ax, 0feffh
        push    ax
        popf
        cli
        mov     ax, cs
        mov     ds, ax
        mov     ss, ax
        mov     sp, savesp
        sti
        mov     ah, 4dh                 ; child's return code
        int     21h
        mov     retcode, al
restore:
        push    ds
        lds     dx, old1
        mov     ax, 2501h
        int     21h
        pop     ds
        push    ds
        lds     dx, old3
        mov     ax, 2503h
        int     21h
        pop     ds
        ; print the log
        mov     si, offset logbuf
pr_loop:
        cmp     si, logptr
        jae     pr_done
        mov     ax, [si]
        call    hex4
        mov     dl, '='
        call    putc
        mov     al, [si+2]
        call    hex2
        mov     dl, ' '
        call    putc
        add     si, 3
        inc     col
        cmp     col, 8
        jb      pr_loop
        mov     col, 0
        call    crlf
        jmp     pr_loop
pr_done:
        call    crlf
        mov     dx, offset msg_n
        mov     ah, 9
        int     21h
        mov     ax, logptr
        sub     ax, offset logbuf
        xor     dx, dx
        mov     bx, 3
        div     bx
        call    hex4
        cmp     overflow, 0
        je      no_ovf
        mov     dx, offset msg_ovf
        mov     ah, 9
        int     21h
no_ovf:
        mov     dx, offset msg_rc
        mov     ah, 9
        int     21h
        mov     al, retcode
        call    hex2
        mov     dx, offset msg_st
        mov     ah, 9
        int     21h
        mov     ax, word ptr steps+2
        call    hex4
        mov     ax, word ptr steps
        call    hex4
        call    crlf
        mov     ax, 4c00h
        int     21h

; single-step handler: look at the instruction about to execute
int1:
        add     word ptr cs:steps, 1
        adc     word ptr cs:steps+2, 0
        push    bp
        mov     bp, sp
        push    ax
        push    si
        push    ds
        lds     si, [bp+2]
        mov     ax, ds                  ; outside UNISOUND (in DOS or the BIOS):
        cmp     ax, cs:childpsp         ; stop tracing until the INT 3 below
        je      i1_child
        and     word ptr [bp+6], 0feffh
        jmp     i1_done
i1_child:
        cmp     byte ptr [si], 0cdh     ; INT n: DOS may return with RETF 2 and
        jne     i1_out                  ; lose TF, so plant an INT 3 after it
        add     si, 2
        mov     al, [si]
        mov     cs:bp_byte, al
        mov     word ptr cs:bp_addr, si
        mov     word ptr cs:bp_addr+2, ds
        mov     byte ptr [si], 0cch
        jmp     i1_done
i1_out:
        cmp     byte ptr [si], 0eeh     ; OUT DX,AL
        jne     i1_done
        cmp     dx, 279h
        je      i1_log
        cmp     dx, 0a79h
        je      i1_log
        cmp     dx, 660h
        je      i1_log
        cmp     dx, 661h
        je      i1_log
        cmp     dx, 800h
        je      i1_log
        cmp     dx, 801h
        je      i1_log
        jmp     i1_done
i1_log:
        mov     si, cs:logptr
        cmp     si, offset logend
        jb      i1_room
        mov     cs:overflow, 1
        jmp     i1_done
i1_room:
        mov     cs:[si], dx
        mov     al, [bp-2]              ; the AL being written (saved AX)
        mov     cs:[si+2], al
        add     si, 3
        mov     cs:logptr, si
i1_done:
        pop     ds
        pop     si
        pop     ax
        pop     bp
        iret

; INT 3 planted after an INT n: put the byte back, rerun it, trace again
int3:
        push    bp
        mov     bp, sp
        push    ax
        push    si
        push    ds
        lds     si, cs:bp_addr
        mov     al, cs:bp_byte
        mov     [si], al
        dec     word ptr [bp+2]
        or      word ptr [bp+6], 100h
        pop     ds
        pop     si
        pop     ax
        pop     bp
        iret

hex4:   push    ax
        mov     al, ah
        call    hex2
        pop     ax
hex2:   push    ax
        shr     al, 1
        shr     al, 1
        shr     al, 1
        shr     al, 1
        call    hex1
        pop     ax
hex1:   and     al, 0fh
        add     al, '0'
        cmp     al, '9'
        jbe     h1
        add     al, 7
h1:     mov     dl, al
putc:   push    ax
        mov     ah, 2
        int     21h
        pop     ax
        ret
crlf:   mov     dl, 13
        call    putc
        mov     dl, 10
        jmp     putc

fname   db      'UNISOUND.COM', 0
msg_load db     'cannot load UNISOUND.COM', 13, 10, '$'
msg_n   db      'writes: $'
msg_ovf db      ' (log full)$'
msg_st  db      ', steps: $'
msg_rc  db      13, 10, 'exit code: $'
pblock  dw      0
pb_tail dw      80h, 0
pb_fcb1 dw      5ch, 0
pb_fcb2 dw      6ch, 0
pb_sssp dd      0
pb_csip dd      0
old1    dd      0
old3    dd      0
bp_addr dd      0
bp_byte db      0
childpsp dw     0
savesp  dw      0
retcode db      0
overflow db     0
steps   dd      0
col     db      0
logptr  dw      offset logbuf
        even
        db      512 dup (0)
stktop  label   word
logbuf  db      12000 dup (?)
logend  label   byte
        end     start
