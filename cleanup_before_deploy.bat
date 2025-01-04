@echo off
echo [32mCleaning up unnecessary files...[0m

:: Remove Render-specific files
del /q /f render.yaml 2>nul
del /q /f httpserver.service 2>nul

:: Remove development files
del /q /f CMakeLists.txt 2>nul
del /q /f HTTPThreadServer.cbp 2>nul
del /q /f deploy.bat 2>nul
del /q /f *.o 2>nul
del /q /f *.exe 2>nul
del /q /f start_server.bat 2>nul

:: Remove temporary directories
rd /s /q bin 2>nul
rd /s /q obj 2>nul
rd /s /q build 2>nul
rd /s /q Debug 2>nul
rd /s /q Release 2>nul
rd /s /q .vs 2>nul
rd /s /q .vscode 2>nul

:: Remove git files if they exist
rd /s /q .git 2>nul
del /q /f .gitignore 2>nul

:: Remove any backup files
del /q /f *.bak 2>nul
del /q /f *~ 2>nul

:: Keep only essential files for Fly.io deployment:
echo [32mKeeping essential files for deployment:[0m
echo.
echo [36mSource files:[0m
echo   - example.c
echo   - httplibrary.c
echo   - httplibrary.h
echo   - string_lib.c
echo   - string_lib.h
echo.
echo [36mConfiguration files:[0m
echo   - Dockerfile
echo   - fly.toml
echo   - config.json
echo   - cleanup.bat (for file expiry)
echo   - README.md
echo.
echo [32mCleanup complete! Your project is ready for deployment to Fly.io[0m

pause 