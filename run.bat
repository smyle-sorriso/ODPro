@echo off
cd /d "%~dp0"

set "EXE_PATH="
for /f "delims=" %%F in ('dir /b /a-d /o-d ".\build\*.exe" 2^>nul') do (
    set "EXE_PATH=.\build\%%F"
    goto :found
)

echo Nenhum executavel encontrado em .\build
exit /b 1

:found
start "" "%EXE_PATH%"
