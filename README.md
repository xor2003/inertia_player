MASM x86 to C translator 0.8
==============
Translator may be used to port your DOS assembler code to SDL C. Since there is no working decompiler for 16 bit DOS code yet.
Your porting path:
DOS binary 16 bit real or 32 bit protected modes -> IDA Pro (disassembly) -> export assembler (masm format) -> Tasmx to compile (dosbox debugger to fix) -> this MASM to C translator -> C SDL source.

Optionally you can recompile resulting C to 32 bits with optimization and recompile to get cleaner C code without dead code and flags update.

Key features:
- Most of x86 instructions (except FPU) are supported (well tested with QEMU tests).
flags: Carry, Zero, Sign are supported for most of x86 instructions.
- Segment memory model and 16bit offsets.
- Some BIOS/DOS Int 10h, 21h interrupts are supported.
  Also DOS memory manager and stack.
- CGA text mode is supported using Curses (PDcurses or NCurses).
- VGA 320x200x256 support (partial)

3 party code used: ASM2C (x86 instruction emulation), tasm-recover (from SCUMMVM project; highly modified), QEMU x86 test suit, FreeDOS memory manager.

License: GPL2.

Sure ASM2C have cleaner code and good parser but it written on Swift and less instructions supported, also only protected mode.

TODO: 
- equ out of segment/proceedure
- macros support
- full VGA/EGA
- better parser based on Masm EBNF
- proper SB emulation or use DOSBOX as library
- add FPU instructions support
- better keyboard

Execute to convert your masm 16 bit source to C:
-------------------------------

masm-recover <some.asm>

(Some small resulting source code modification will be also required to build)

IDA Pro Free https://www.scummvm.org/news/20180331/

Inertia Player 1.22 source code
=============
Famous reverse engenerred MOD, S3M player.
Currently platform DOS (ASM), SDL2 (C++)
There is source code for MASM, TASM Ideal and Nasm, gcc which can be built and running

TODO: fix sound on SDL, finish porting (keyboard, graphics mode,...)

Binary is available on releases page
-------------------------------

Building Inertia for SDL from source:
-------------------------------
1. get PDCurses or other curses library+headers, SDL2, mingw32
2. build_mingw.bat
3. execute:
iplay_masm.exe HACKER4.S3M 


