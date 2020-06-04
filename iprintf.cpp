#include <cstdio>
#include <cstdlib>
#include <stdint.h>

typedef uint32_t dd;

dd iiprintf(dd stackPointer)
{
dd save = *(dd*)stackPointer;
__asm__(  ".intel_syntax;\n"
        "push    ebp\n"     //backup ebp
 "       mov     ebp,esp\n" // backup real stack

"	mov     esp, %[stackPointer]\n" // switch to emulated stack. printf arguments are stored as cdecl in stack (not in regs)
"	add	esp, 4\n" // remove it from stack
 "       call    _printf\n"

  "      mov     esp, ebp\n" // restore real stack
   "     pop     ebp\n"

//"	ret\n"
  ".att_syntax;"
  :
  :[stackPointer] "r" (stackPointer)
 );

 *(dd*)stackPointer = save;
 return stackPointer;
}
