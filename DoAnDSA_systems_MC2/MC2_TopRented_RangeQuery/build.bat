@echo off
REM build.bat - Bien dich module MC2 tren Windows bang MinGW-w64 (g++)
REM Yeu cau: da cai dat MinGW-w64 va them "g++" vao PATH (xem README.md muc "Cai dat").
REM Cach dung:
REM     build.bat
REM Sau khi build xong se tao ra file: mc2_app.exe (o thu muc goc du an)

echo ============================================
echo   Dang bien dich module MC2 (C++17)...
echo ============================================

g++ -std=c++17 -O2 -Wall -Wextra -o mc2_app.exe src\main.cpp src\CSVUtils.cpp src\RentalService.cpp

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [LOI] Bien dich that bai. Kiem tra lai da cai g++ chua ^(xem README.md^).
    exit /b 1
)

echo.
echo ============================================
echo   BUILD THANH CONG: mc2_app.exe
echo   Chay bang lenh:  mc2_app.exe
echo ============================================
