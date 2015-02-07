@rem Check on prerequisites, sdcc >3.4.0 and python
@if not {%PYTHONDIR%} == {} (
    @if exist %PYTHONDIR%\python.exe (
        @goto :pythonfound
    )
)
@rem Python dir not set or python.exe not found, look in PATH
@for %%p in (python.exe) do @set PYTHONDIR="%%~dp$PATH:p"
@if {%PYTHONDIR%} == {""} (
    @echo Could not find Python on your PATH or in PYTHONDIR!
    @echo Please set PYTHONDIR to the full python folder, or add this to PATH.
    @echo Example: set "PATH=%%PATH%%;C:\Program Files (x86)\Python"
    @echo Example: set PYTHONDIR="C:\Program Files (x86)\Python"
    @exit /b
)
:pythonfound
@echo Found python.exe at %PYTHONDIR%

@rem Look for SDCC
@if not {%SDCCDIR%} == {} (
    @if exist %SDCCDIR%\sdcc.exe (
        @goto :sdccfound
    )
)
@rem SDCC dir not set or sdcc.exe not found, look in PATH
@for %%s in (sdcc.exe) do @set SDCCDIR="%%~dp$PATH:s"
@if {%SDCCDIR%} == {""} (
    @echo Could not find SDCC on your PATH or in SDCCDIR!
    @echo Please set SDCCDIR to the full path to SDCC\bin, or add this to PATH.
    @echo Example: set "PATH=%%PATH%%;C:\Program Files (x86)\SDCC\bin"
    @echo Example: set SDCCDIR="C:\Program Files (x86)\SDCC\bin"
    @exit /b
)
:sdccfound

@rem Parse the output of "sdcc --version" and look for an X.Y.Z version string
@for /f "usebackq tokens=1-10" %%a in (`call %SDCCDIR%\sdcc.exe -v`) do @(
    @for %%m in (%%a %%b %%c %%d %%e %%f %%g %%h %%i %%j) do @(
        @echo %%m | findstr /r "[0-9]*\.[0-9]*\.[0-9]" > NUL 2>&1
        @if not ERRORLEVEL 1 (
            @set SDCCVER=%%m
            @goto :sdccversion
        )
    )
)
@echo Found sdcc.exe, but could not determine version!
@echo This program depends on SDCC 3.4.0 or newer.
@echo The sdcc.exe in question is in %SDCCDIR%
@exit /b
:sdccversion
@echo Found sdcc.exe version %SDCCVER% in %SDCCDIR%
@rem From version X.Y.Z, make sure X is at least 3.
@rem If it is exactly 3, make sure Y is at least 4.
@for /f "tokens=1-3 delims=." %%a in ("%SDCCVER%") do @(
    @if %%a geq 3 (
        @if %%a equ 3 (
            @if %%b lss 4 (
                @echo Detected SDCC minor version less than 4.
                @goto :sdcctooold
            )
        )
    ) else (
        @echo Detected SDCC major version less than 3.
        @goto :sdcctooold
    )
    @goto :sdccok
)
:sdcctooold
@echo This program requires a minimum of SDCC 3.4.0.
@exit /b
:sdccok

@if {%CALC%} == {} (
    @set CALC=ti86
)

@set SDCC=%SDCCDIR%\sdcc.exe
@set SDAS=%SDCCDIR%\sdasz80.exe
@set PYTHON=%PYTHONDIR%\python.exe
@set LINKS=Build\%CALC%_crt0.rel
@set BUILD=@call :build
@set COMPILE=@call :compile
@set FAKE=@call :fake
@set SDCCBASE=%SDCC% -mz80 --no-std-crt0 --reserve-regs-iy --opt-code-speed -ISource -ISource\%CALC%

@mkdir Build > NUL 2>&1
%SDAS% -p -g -o Build\%CALC%_crt0.rel Source\%CALC%\crt0.s

%COMPILE% Source\bitmap.c
%COMPILE% Source\camera.c
%COMPILE% Source\game.c
%COMPILE% Source\kibble.c
%COMPILE% Source\kitty.c
%COMPILE% Source\lander.c
%COMPILE% Source\lock.c
%COMPILE% Source\menu.c
%COMPILE% Source\moon.c
%COMPILE% Source\physics.c
%COMPILE% Source\%CALC%\%CALC%.c
%BUILD% Source\main.c

@rem SDCC generates main.ihx instead of main.c.ihx?
%PYTHON% Tools\ihxtobin.py Build\Source\main.ihx

@rem Patch the binary to initialize globals. Usually SDCC expects this to be
@rem done in the startup code, but this is not necessary on a TI.
%PYTHON% Tools\trim.py Build\Source\main.c.map Build\Source\main.ihx.bin 0xD748
@copy /y Build\Source\main.ihx.bin Build\Source\catlandr.bin
%PYTHON% Tools\binto86p.py Build\Source\catlandr.bin

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
