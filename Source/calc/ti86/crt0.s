; tios_crt0.s - TIOS assembly program header 
   .module tios_crt0 
   .globl _main
   .area _HEADER (ABS) 
   .org 0xD748
   call _main
   im 1
   call _savescores
   ret
