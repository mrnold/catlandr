; tios_crt0.s - TIOS assembly program header 
   .module tios_crt0 
   .globl _startup
   .globl _main
   .area _CODE
   _startup:
      call _gsinit
      call _main
      im 1
      ret
   .area _GSINIT
   _gsinit:
   .area _GSFINAL
      ret
