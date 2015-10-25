#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <intrinsics.h>

#define enableInterrupts()    __enable_interrupt()  /* enable interrupts */
#define disableInterrupts()   __disable_interrupt() /* disable interrupts */
#define rim()                 __enable_interrupt()  /* enable interrupts */
#define sim()                 __disable_interrupt() /* disable interrupts */
#define nop()                 __no_operation()      /* No Operation */
#define trap()                __trap()              /* Trap (soft IT) */
#define wfi()                 __wait_for_interrup() /* Wait For Interrupt */
#define halt()                __halt()              /* Halt */

#endif
