#include <aarch64/asm.h>

/* Initial boot environment is described in detail 
 * at Documentation/arm64/booting.rst from Linux source tree. */

#define IMAGE_OFFSET 0 /* Image offset from start of 2 MiB page */
#define IMAGE_SIZE __kernel_size
#define IMAGE_FLAGS 2 /* Kernel is little endian, page size is 4KiB */

        .section .boot, "ax"

_ENTRY(_start)
        /* Based on locore.S from FreeBSD. */
        b       1f
        .long   0
        .quad   IMAGE_OFFSET
        .quad   IMAGE_SIZE
        .quad   IMAGE_FLAGS
        .quad   0
        .quad   0
        .quad   0
        .long   0x644d5241 /* Magic "ARM\x64" */
        .long   0
1:
        /* Get CPU number. */
        MRS     x3, MPIDR_EL1
        AND     x3, x3, #3
        CMP     x3, #0
        BNE     .

        /* Save pointer to dtb. */
        mov     x19, x0
        /* Setup initial stack. */
        ADR     x3, __boot_stack_end
        MOV     sp, x3

        BL      aarch64_init
        /* 
         * Temporary stack in VA.
         * It's needed because in rpi3.c accesses to stack are instrumented
         * and KASAN works only for virtual addresses.
         * Stack instrumentation is done directly by GCC not by our code so
         * we can't disable KASAN for that file in a simple way because we call
         * functions from libkern.
         */
        mov     sp, x0
        /* Restore dtb pointer. */
        mov     x0, x19

        BL      board_stack
        MOV     sp, x0

        B       board_init
_END(_start)

        .section .boot.data
        .globl  __boot_stack

        .align  4
__boot_stack:
        .space  1024
__boot_stack_end:

# vim: sw=8 ts=8 et
