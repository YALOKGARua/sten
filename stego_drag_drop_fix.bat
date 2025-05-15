@echo off
chcp 1251 > nul
title G-let D3 PGM Steganography - Drag & Drop
color 0A

REM Check if a file was provided via drag & drop
if [%1]==[] goto no_file

set DROPPED_FILE=%~1
set FILE_EXT=%~x1

REM Check if the file is a PGM file
if /i not "%FILE_EXT%"==".pgm" (
    echo Error: The dropped file must be a PGM file.
    echo Dropped file: %DROPPED_FILE%
    echo File extension: %FILE_EXT%
    pause
    goto wait_and_exit
)

:select_operation
cls
echo =======================================
echo  G-let D3 PGM Steganography - Drag ^& Drop
echo =======================================
echo.
echo Dropped file: %DROPPED_FILE%
echo.
echo What operation would you like to perform?
echo.
echo 1. Use as cover image (to hide a secret image)
echo 2. Use as secret image (to be hidden)
echo 3. Use as stego image (to extract a hidden image)
echo 4. Use as reference for quality assessment
echo 5. Exit
echo.
set /p operation=Enter choice (1-5): 

if "%operation%"=="1" goto use_as_cover
if "%operation%"=="2" goto use_as_secret
if "%operation%"=="3" goto use_as_stego
if "%operation%"=="4" goto use_as_reference
if "%operation%"=="5" goto wait_and_exit
echo Invalid choice! Press any key to try again...
pause > nul
goto select_operation

:use_as_cover
cls
echo =======================================
echo  Using as Cover Image (to hide a secret)
echo =======================================
echo.
echo Cover image: %DROPPED_FILE%
echo.
set /p secret=Enter secret image path (.pgm): 
if not exist "%secret%" (
    echo Error: Secret image file not found.
    pause
    goto select_operation
)
set /p output=Enter output stego image path (.pgm): 
set /p blocksize=Enter block size (power of 2, default 8): 
set /p strength=Enter embedding strength (1-10, default 5): 
set /p random=Use random block selection for security? (y/n): 

set command=.\bin\stego.exe embed "%DROPPED_FILE%" "%secret%" "%output%"

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
goto select_operation

:use_as_secret
cls
echo =======================================
echo  Using as Secret Image (to be hidden)
echo =======================================
echo.
echo Secret image: %DROPPED_FILE%
echo.
set /p cover=Enter cover image path (.pgm): 
if not exist "%cover%" (
    echo Error: Cover image file not found.
    pause
    goto select_operation
)
set /p output=Enter output stego image path (.pgm): 
set /p blocksize=Enter block size (power of 2, default 8): 
set /p strength=Enter embedding strength (1-10, default 5): 
set /p random=Use random block selection for security? (y/n): 

set command=.\bin\stego.exe embed "%cover%" "%DROPPED_FILE%" "%output%"

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
goto select_operation

:use_as_stego
cls
echo =======================================
echo  Using as Stego Image (to extract hidden data)
echo =======================================
echo.
echo Stego image: %DROPPED_FILE%
echo.
set /p output=Enter output secret image path (.pgm): 
set /p dimensions=Do you know secret image dimensions? (y/n): 

set command=.\bin\stego.exe extract "%DROPPED_FILE%" "%output%"

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
goto select_operation

:use_as_reference
cls
echo =======================================
echo  Using as Reference for Quality Assessment
echo =======================================
echo.
echo Reference image: %DROPPED_FILE%
echo.
set /p modified=Enter modified/stego image to compare with (.pgm): 
if not exist "%modified%" (
    echo Error: Modified image file not found.
    pause
    goto select_operation
)

set command=.\bin\stego.exe assess "%DROPPED_FILE%" "%modified%"

echo.
echo Executing: %command%
echo.
%command%
echo.
echo Operation completed.
pause
goto select_operation

:no_file
cls
echo =======================================
echo  G-let D3 PGM Steganography - Drag ^& Drop
echo =======================================
echo.
echo Please drag and drop a PGM file onto this batch file.
echo.
echo Usage instructions:
echo 1. Drag a PGM file onto this batch file
echo 2. Select the operation to perform with the file
echo 3. Enter the required parameters
echo.
echo Examples:
echo - Drag a cover image to embed a secret image inside it
echo - Drag a stego image to extract the hidden secret image
echo - Drag an original image to assess quality against a stego version
echo.
pause
goto wait_and_exit

:wait_and_exit
echo.
echo Press any key to exit...
pause > nul
exit /b 