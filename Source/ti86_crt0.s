; tios_crt0.s - TIOS assembly program header 
   .module tios_crt0 
   .globl _main
   .area _HEADER (ABS) 
   .org 0xD746
   .db 0x8E, 0x28
   call _main
   im 1
   ret
