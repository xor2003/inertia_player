MASM x86 to C translator 0.91
==============
(Portable MASM)
Translator may be used to port your DOS x86 (MASM) assembler code to SDL C. (Since there is no working decompiler for 16 bit DOS code yet.
Because of DOS segmentation model, etc)

Translator generates pseudo assembler instruction which can be compiled with C compiler.

Your porting path:

0. DOS binary 16 bit real or 32 bit protected modes -> 
1. IDA Pro (disassembly) -> export assembler (masm format) -> 
2. uasm to compile for dos (dosbox debugger to fix) -> 
3. this MASM to C translator -> 
4. C SDL source.

Optionally you can compile output of masm2c (C code) for 32 bit plarform with optimization 
and decompile to get cleaner C code without dead code like x86 flags update.

Key features:
- Most of x86 instructions (except FPU) are supported (well tested with QEMU tests).
flags: Carry, Zero, Sign are supported for most of x86 instructions.
- Segment memory model and 16bit offsets.
- Some BIOS/DOS Int 10h, 21h interrupts are supported. (Maybe I will reuse DOSBOX implementations in future)
  Also DOS memory manager and stack.
- CGA text mode is supported using Curses (PDcurses or NCurses).
- VGA 320x200x256 support (partial)

3rd-party code used from: ASM2C (x86 instruction emulation), tasm-recover (from SCUMMVM project; highly modified), QEMU x86 instructions test suit, FreeDOS memory manager.

License: GPL2.

Sure ASM2C have cleaner code and good parser but it written on Swift and less instructions supported, also only protected mode.

TODO: 
- equ out of segment/proceedure
- macros support
- full VGA/EGA
- better parser based on Masm EBNF
- proper SB emulation or use DOSBOX as library
- better keyboard
- add FPU instructions support (low priority)

Also to easier disassembling (exe to asm) need to collect run-time information from dosbox or other emulator to annotate IDA disassembly. Maybe modify dosbox debugger tracing mechanism to collect:
- current instruction is code
- access to memory (which segment), offset which segment
- segement register value
In other repo there are scripts which can help to convert DOSBOX run-time traces into IDA annotations to simplify disassembly.

To build resulting disassembly I use uasm(jwasm)/masm6, link5/tlink. See uasm project \Samples\Dos\ for examples.

See list of DOS games with debug information http://bringerp.free.fr/forum/viewtopic.php?f=1&t=128

How to run to convert your masm 16 bit source to C:
-------------------------------

masm2c.py <some.asm>

(Some source code modification might also required to build)

IDA Pro Free https://www.scummvm.org/news/20180331/

Inertia Player 1.22 source code
=============
Famous reverse engenerred MOD, S3M player.
Currently platform DOS (ASM), SDL2 (C)
There is disassembled source code for MASM, Nasm, C which can be built and running

TODO: finish sound supprt on SDL, finish porting (keyboard, graphics mode,...)

Binary is available on releases page
-------------------------------

Building Inertia for SDL from source:
-------------------------------
1. get PDCurses or other curses library+headers, SDL2, mingw32 + a lot of luck
2. build_mingw.bat
3. execute:
iplay_m_.exe HACKER4.S3M 

Or just get prebuilt
