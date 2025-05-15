@echo off
title G-let D3 PGM Steganography Tool
color 0A

:main_menu
cls
echo =======================================
echo    G-let D3 PGM Steganography Tool    
echo =======================================
echo.
echo  1. Embed Secret Image
echo  2. Extract Secret Image
echo  3. Assess Image Quality
echo  4. Exit
echo.
set /p choice=Enter your choice (1-4): 

if "%choice%"=="1" goto embed
if "%choice%"=="2" goto extract
if "%choice%"=="3" goto assess
if "%choice%"=="4" goto exit
echo Invalid choice! Press any key to try again...
pause > nul
goto main_menu

:embed
cls
echo =======================================
echo          Embed Secret Image          
echo =======================================
echo.
set /p cover=Enter cover image path (.pgm): 
set /p secret=Enter secret image path (.pgm): 
set /p output=Enter output stego image path (.pgm): 
set /p blocksize=Enter block size (power of 2, default 8): 
set /p strength=Enter embedding strength (1-10, default 5): 
set /p random=Use random block selection for security? (y/n): 

set command=.\bin\stego.exe embed "%cover%" "%secret%" "%output%"

if not "%blocksize%"=="" (
    set command=%command% -b %blocksize%
)

if not "%strength%"=="" (
    set command=%command% -s %strength%
)

if /i "%random%"=="y" (
    set command=%command% -r
    set /p seed=Enter random seed (default: 12345): 
    if not "%seed%"=="" (
        set command=%command% -seed %seed%
    )
)

echo.
echo Executing: %command%
echo.
%command%
echo.
echo Operation completed.
pause
goto main_menu

:extract
cls
echo =======================================
echo         Extract Secret Image         
echo =======================================
echo.
set /p stego=Enter stego image path (.pgm): 
set /p output=Enter output secret image path (.pgm): 
set /p dimensions=Do you know secret image dimensions? (y/n): 

set command=.\bin\stego.exe extract "%stego%" "%output%"

if /i "%dimensions%"=="y" (
    set /p width=Enter secret image width: 
    set /p height=Enter secret image height: 
    set command=%command% %width% %height%
)

set /p blocksize=Enter block size (power of 2, default 8): 
set /p strength=Enter embedding strength (1-10, default 5): 
set /p random=Was random block selection used? (y/n): 

if not "%blocksize%"=="" (
    set command=%command% -b %blocksize%
)

if not "%strength%"=="" (
    set command=%command% -s %strength%
)

if /i "%random%"=="y" (
    set command=%command% -r
    set /p seed=Enter the same random seed used for embedding: 
    if not "%seed%"=="" (
        set command=%command% -seed %seed%
    )
)

echo.
echo Executing: %command%
echo.
%command%
echo.
echo Operation completed.
pause
goto main_menu

:assess
cls
echo =======================================
echo          Assess Image Quality        
echo =======================================
echo.
set /p original=Enter original image path (.pgm): 
set /p modified=Enter modified/stego image path (.pgm): 

set command=.\bin\stego.exe assess "%original%" "%modified%"

echo.
echo Executing: %command%
echo.
%command%
echo.
echo Operation completed.
pause
goto main_menu

:exit
cls
echo Thank you for using G-let D3 PGM Steganography Tool!
timeout /t 2 > nul
exit 