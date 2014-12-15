; tios_crt0.s - TIOS assembly program header 
   .module tios_crt0 
   .globl _main
   .globl _timer_isr
   .area _HEADER (ABS) 
   .org 0xD746
   .db 0x8E, 0x28
   call gsinit
   call _main
   im 1
   ret
   .area _GSINIT
gsinit::
   ld bc, #l__INITIALIZER
   ld a, b
   or a, c
   jr z, gsinit_done
   ld de, #s__INITIALIZED
   ld hl, #s__INITIALIZER
   ldir
gsinit_done:
   .area _GSFINAL
   ret
