@echo off
REM ========================================
REM  SCRIPT DE COMPILACIÓN AUTOMÁTICA
REM  Juego de Exploración - Erick y Esteban
REM ========================================

echo.
echo ========================================
echo   COMPILANDO JUEGO DE EXPLORACION
echo ========================================
echo.

REM Compilar el archivo principal
echo [1/3] Compilando GAME.asm...
tasm /zi GAME.asm
if errorlevel 1 goto error

echo.
echo [2/3] Enlazando archivos...
tlink /v GAME.obj
if errorlevel 1 goto error

echo.
echo [3/3] Limpiando archivos temporales...
if exist GAME.obj del GAME.obj
if exist GAME.map del GAME.map

echo.
echo ========================================
echo   COMPILACION EXITOSA!
echo ========================================
echo.
echo El juego ha sido compilado exitosamente.
echo Ejecuta GAME.exe para jugar.
echo.
pause
goto end

:error
echo.
echo ========================================
echo   ERROR EN LA COMPILACION
echo ========================================
echo.
echo Verifica que:
echo - TASM este instalado y en el PATH
echo - Todos los archivos .INC esten presentes
echo - No haya errores de sintaxis
echo.
pause

:end
