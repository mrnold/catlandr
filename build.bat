@if {%1} == {} (
    @echo Calculator not specified, building for TI-86.
    @set CRTSIZE=9
    @set MODEL=86
    @set BASE=0xD748
    @set CALC=ti86
    @set EXT=86p
) else if {%1} == {86} (
    @echo Building for TI-86.
    @set CRTSIZE=9
    @set MODEL=86
    @set BASE=0xD748
    @set CALC=ti86
    @set EXT=86p
) else if {%1} == {84pse} (
    @echo Building for TI-84 Plus Silver Edition.
    @set CRTSIZE=6
    @set MODEL=8402
    @set BASE=0x9D95
    @set CALC=ti84pse
    @set EXT=8xp
) else (
    @echo Unsupported calculator specified: %1
    @echo Supported model options: 86 84pse
    @exit /b
)
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

@set SDCC=%SDCCDIR%\sdcc.exe
@set SDAS=%SDCCDIR%\sdasz80.exe
@set PYTHON=%PYTHONDIR%\python.exe
@set LINKS=Build\%CALC%_crt0.rel
@set BUILD=@call :build
@set COMPILE=@call :compile
@set FAKE=@call :fake
@set SDCCBASE=%SDCC% -mz80 --no-std-crt0 --reserve-regs-iy --opt-code-speed --max-allocs-per-node 30000 -ISource -DCALCULATOR_MODEL=%MODEL%

@mkdir Build > NUL 2>&1
%SDAS% -p -g -o Build\%CALC%_crt0.rel Source\calc\%CALC%\crt0.s || goto :failed

%COMPILE% Source\bitmap.c || goto :failed
%COMPILE% Source\camera.c || goto :failed
%COMPILE% Source\game.c || goto :failed
%COMPILE% Source\kibble.c || goto :failed
%COMPILE% Source\kitty.c || goto :failed
%COMPILE% Source\lander.c || goto :failed
%COMPILE% Source\menu.c || goto :failed
%COMPILE% Source\moon.c "--max-allocs-per-node 3000" || goto :failed
%COMPILE% Source\physics.c || goto :failed
%COMPILE% Source\calc\%CALC%\%CALC%.c || goto :failed
%BUILD% Source\main.c || goto :failed

@rem SDCC generates main.ihx instead of main.c.ihx?
%PYTHON% Tools\ihxtobin.py Build\Source\main.ihx

@rem Patch the binary to initialize globals. Usually SDCC expects this to be
@rem done in the startup code, but this is not necessary on a TI.
%PYTHON% Tools\trim.py Build\Source\main.c.map Build\Source\main.ihx.bin %BASE%
@copy /y Build\Source\main.ihx.bin Build\Source\catlandr.bin
%PYTHON% Tools\binto86p.py %CALC% Build\Source\catlandr.bin

@copy /y Build\Source\catlandr.%EXT% catlandr.%EXT%
@goto :done

:compile
@mkdir Build\%1 > NUL 2>&1
@rmdir Build\%1 > NUL 2>&1
%SDCCBASE% %2 -c %1 -o Build\%1.rel
@set LINKS=%LINKS% Build\%1.rel
@goto :done

:fake
@set LINKS=%LINKS% Build\%1.rel
@goto :done

:build
@mkdir Build\%1 > NUL 2>&1
@rmdir Build\%1 > NUL 2>&1
@set /a CODELOC=BASE+CRTSIZE
%SDCCBASE% --out-fmt-ihx -o Build\%1.ihx --data-loc 0 --code-loc %CODELOC% %LINKS% %1
@goto :done

:failed
@echo Build failed!

:done
