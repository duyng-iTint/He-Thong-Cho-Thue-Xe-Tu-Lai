@echo off
REM run_demo.bat - Chay nhanh chuong trinh voi bo du lieu demo nho (60 don thue)
REM Gia dinh ban da chay build.bat o thu muc goc truoc do.

cd /d %~dp0\..

if not exist mc2_app.exe (
    echo [LOI] Khong tim thay mc2_app.exe - hay chay build.bat truoc.
    exit /b 1
)

echo ============================================
echo   DEMO NHANH - MODULE MC2
echo   Du lieu: data\donthue_xe.csv (60 don thue mau)
echo ============================================
mc2_app.exe
