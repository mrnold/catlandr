@mkdir Build > NUL 2>&1
@sdasz80 -p -g -o Build\ti86_crt0.rel Source\ti86_crt0.s

@set LINKS=Build\ti86_crt0.rel
@set BUILD=@call :build
@set COMPILE=@call :compile
@set FAKE=@call :fake
@set SDCCBASE=sdcc -mz80 --no-std-crt0 --reserve-regs-iy --opt-code-speed -ISource

%COMPILE% Source\bitmap.c
%COMPILE% Source\kibble.c
%COMPILE% Source\kitty.c
%COMPILE% Source\lander.c
%COMPILE% Source\lock.c
%COMPILE% Source\menu.c
%COMPILE% Source\moon.c
%COMPILE% Source\physics.c
%COMPILE% Source\ti86.c
%BUILD% Source\main.c

@rem SDCC generates main.ihx instead of main.c.ihx?
python Tools\ihxtobin.py Build\Source\main.ihx

@rem Patch the binary to initialize globals. Usually SDCC expects this to be
@rem done in the startup code, but this is not necessary on a TI.
python Tools\trim.py Build\Source\main.c.map Build\Source\main.ihx.bin 0xD748
@copy /y Build\Source\main.ihx.bin Build\Source\catlandr.bin
python Tools\binto86p.py Build\Source\catlandr.bin

@copy /y Build\Source\catlandr.86p catlandr.86p
@goto :done

:compile
@mkdir Build\%1 > NUL 2>&1
@rmdir Build\%1 > NUL 2>&1
%SDCCBASE% -c %1 -o Build\%1.rel
@set LINKS=%LINKS% Build\%1.rel
@goto :done

:fake
@set LINKS=%LINKS% Build\%1.rel
@goto :done

:build
@mkdir Build\%1 > NUL 2>&1
@rmdir Build\%1 > NUL 2>&1
%SDCCBASE% --out-fmt-ihx -o Build\%1.ihx --data-loc 0 --code-loc 0xD74E %LINKS% %1
@goto :done

:done
