@echo off
echo ===============================
echo Syncing backend JSON to frontend
echo ===============================

:: Copy beds.json
if exist backend\data\beds.json (
    copy /Y backend\data\beds.json frontend\public\beds.json > nul
    echo [OK] Copied beds.json
) else (
    echo [ERROR] backend\data\beds.json not found.
)

:: Copy events.json
if exist backend\data\events.json (
    copy /Y backend\data\events.json frontend\public\events.json > nul
    echo [OK] Copied events.json
) else (
    echo [WARNING] backend\data\events.json not found - maybe no events logged yet.
)

echo.
echo Sync complete.
pause
