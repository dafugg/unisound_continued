import sys
from capstone import *
data=open(sys.argv[1],'rb').read()
a=int(sys.argv[2],16); n=int(sys.argv[3])
md=Cs(CS_ARCH_X86,CS_MODE_16); md.skipdata=True
for k,i in enumerate(md.disasm(data[a-0x100:],a)):
    if k>=n: break
    print(f"{i.address:05x}  {i.bytes.hex():14s} {i.mnemonic} {i.op_str}")
