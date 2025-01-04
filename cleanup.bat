@echo off
echo [32mStarting Xplois Upload Cleanup Service...[0m

:loop
:: Remove files older than the specified expiry time
echo [36mChecking for expired files...[0m

:: Remove 1-minute old files
forfiles /p uploads /s /m *.* /d 0 /c "cmd /c if @fdate LSS @now-0:1 del @path" 2>nul

:: Remove 15-minute old files
forfiles /p uploads /s /m *.* /d 0 /c "cmd /c if @fdate LSS @now-0:15 del @path" 2>nul

:: Remove 30-minute old files
forfiles /p uploads /s /m *.* /d 0 /c "cmd /c if @fdate LSS @now-0:30 del @path" 2>nul

:: Remove 1-hour old files
forfiles /p uploads /s /m *.* /d -1 /c "cmd /c del @path" 2>nul

:: Remove empty directories
for /f "delims=" %%d in ('dir /s /b /ad uploads 2^>nul') do rd "%%d" 2>nul

:: Log cleanup activity
echo [32m%date% %time%: Cleanup completed[0m >> logs\cleanup.log

:: Wait for 5 minutes before next cleanup
timeout /t 300 /nobreak > nul
goto loop
