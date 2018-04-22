/* $Id$ */

.global __reset
__reset:
    ldr sp, =_stack
    bl main
    b .

/* EOF */