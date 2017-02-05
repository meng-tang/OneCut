#ifndef Assert
#include <stdio.h>
#ifndef _DEBUG
#define Assert(expr,msg) 
#else
#define Assert(expr,msg) { if (expr) { } else { printf("ASSERT(%s) FAILED:\n    %s\n", #expr, msg); __asm { int 0x3 } } }
#endif
#endif

// The above macro can be used like this:
//
//      Assert(pos >= 1 && pos <= m_size, "List index is out of bounds");
//
// This macro first prints out the following error message to the console:
//
//      ASSERT(pos >= 1 && pos <= m_size) FAILED:
//            List index is out of bounds
//
// then it uses "int 0x3" to signal to the debugger that it should freeze *immediately* 
// at that location, giving a chance to inspect the "call stack" that led to the crash.
