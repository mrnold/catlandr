@mkdir Build > NUL 2>&1
@sdasz80 -p -g -o Build\ti86_crt0.rel Source\ti86_crt0.s

@set LINKS=Build\ti86_crt0.rel
@set BUILD=@call :build
@set COMPILE=@call :compile
@set FAKE=@call :fake
@set SDCCBASE=sdcc -mz80 --no-std-crt0 --reserve-regs-iy --opt-code-speed -ISource

%COMPILE% Source\bitmap.c
%COMPILE% Source\event.c
%COMPILE% Source\lander.c
%COMPILE% Source\lock.c
%COMPILE% Source\misc.c
%COMPILE% Source\moon.c
%COMPILE% Source\physics.c
%COMPILE% Source\ti86.c
%BUILD%   Source\main.c
@rem SDCC generates main.ihx instead of main.c.ihx?
Tools\hex2bin Build\Source\main.ihx
@rem Reminder: binpack8x.py patch: basepath, fnamein = os.path.split(fnamein)
python Tools\binpac8x.py -6 -O LUNALNDR Build\Source\main.bin
@copy /y Build\Source\main.86p luna.86p
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
%SDCCBASE% --out-fmt-ihx -o Build\%1.ihx --data-loc 0 --code-loc 0xD751 %LINKS% %1
@goto :done

:done
