"""Minimal OMF (Borland .OBJ/.LIB) reader: per-module code (LEDATA) with fixup bytes masked."""
import struct, sys

def records(data, off=0, end=None):
    end = len(data) if end is None else end
    while off + 3 <= end:
        t = data[off]; n = struct.unpack_from('<H', data, off+1)[0]
        yield off, t, data[off+3:off+3+n-1]
        off += 3 + n
        if t in (0x8a, 0x8b): return

def idx(b, i):
    if b[i] & 0x80: return ((b[i] & 0x7f) << 8) | b[i+1], i+2
    return b[i], i+1

def modules(data):
    """Yield (name, segments) per module; segments = {segname: (bytearray, mask bytearray)}"""
    if data[0] == 0xf0:      # library
        pagesize = struct.unpack_from('<H', data, 1)[0] + 3
        off = pagesize
    else:
        off = 0; pagesize = 1
    while off < len(data) and data[off] == 0x80:
        name = None; lnames = ['']; segs = []; content = {}; last = None; pubs = []
        o = off
        for ro, t, b in records(data, off):
            o = ro + 3 + len(b) + 1
            if t == 0x80: name = b[1:1+b[0]].decode('latin1')
            elif t == 0x96:
                i = 0
                while i < len(b):
                    l = b[i]; lnames.append(b[i+1:i+1+l].decode('latin1')); i += 1+l
            elif t in (0x98, 0x99):
                i = 1
                if (b[0] >> 5) == 0: i += 3
                i += 2 if t == 0x98 else 4
                si, i = idx(b, i)
                segs.append(lnames[si])
            elif t in (0xa0, 0xa1):
                si, i = idx(b, 0)
                eo = struct.unpack_from('<H' if t == 0xa0 else '<I', b, i)[0]
                i += 2 if t == 0xa0 else 4
                sname = segs[si-1]
                buf, msk = content.setdefault(sname, (bytearray(), bytearray()))
                d = b[i:]
                if len(buf) < eo + len(d):
                    buf.extend(b'\0' * (eo+len(d)-len(buf))); msk.extend(b'\0'*(eo+len(d)-len(msk)))
                buf[eo:eo+len(d)] = d
                for k in range(len(d)): msk[eo+k] = 1
                last = (sname, eo)
            elif t in (0x9c, 0x9d) and last:
                i = 0
                while i < len(b):
                    if b[i] & 0x80:
                        loc = ((b[i] << 8) | b[i+1]); i += 2
                        kind = (loc >> 10) & 0xf; o2 = loc & 0x3ff
                        size = {0: 1, 1: 2, 2: 2, 3: 4, 4: 1, 5: 2, 9: 2, 11: 4, 13: 4}.get(kind, 2)
                        fd = b[i]; i += 1
                        if not fd & 0x80:
                            if (fd >> 4) & 7 < 3: _, i = idx(b, i)
                        if not fd & 0x08: _, i = idx(b, i)
                        if not fd & 0x04: i += 2 if t == 0x9c else 4
                        sname, eo = last
                        for k in range(size): content[sname][1][eo+o2+k] = 2
                    else:   # thread
                        th = b[i]; i += 1
                        if not (th & 0x40) or (th >> 2) & 7 < 3: _, i = idx(b, i)
            elif t in (0x90, 0x91):
                gi, i = idx(b, 0); si, i = idx(b, i)
                if gi == 0 and si == 0: i += 2
                while i < len(b) - 0:
                    l = b[i]; nm = b[i+1:i+1+l].decode('latin1'); i += 1 + l
                    off = struct.unpack_from('<H' if t == 0x90 else '<I', b, i)[0]; i += 2 if t == 0x90 else 4
                    _, i = idx(b, i)
                    pubs.append((nm, segs[si-1] if si else None, off))
            elif t == 0xa2:
                pass  # LIDATA unhandled
        modules.pubs = pubs
        yield name, content
        off = o
        if pagesize > 1: off = (off + pagesize - 1) // pagesize * pagesize
