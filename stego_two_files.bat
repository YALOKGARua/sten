@echo off
title G-let D3 PGM Steganography - Two File Processor
color 0A

REM Check if at least one file was provided
if "%~1"=="" goto usage

REM Get first file
set FILE1=%~1
set FILE1_EXT=%~x1

REM Check file extensions
if /i not "%FILE1_EXT%"==".pgm" (
    echo Error: All files must be PGM files.
    echo File 1: %FILE1% (Extension: %FILE1_EXT%)
    pause
    exit /b 1
)

REM Check if second file was provided
if "%~2"=="" (
    goto one_file
) else (
    set FILE2=%~2
    set FILE2_EXT=%~x2
    
    if /i not "%FILE2_EXT%"==".pgm" (
        echo Error: All files must be PGM files.
        echo File 2: %FILE2% (Extension: %FILE2_EXT%)
        pause
        exit /b 1
    )
    
    goto two_files
)

:one_file
cls
echo =======================================
echo  G-let D3 PGM Steganography - Single File
echo =======================================
echo.
echo Detected one PGM file: %FILE1%
echo.
echo For better results, drag and drop TWO files simultaneously:
echo - Cover image and secret image for embedding
echo - Original image and stego image for quality assessment
echo - Stego image and another file (optional) for extraction
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

if "%operation%"=="1" goto cover_single
if "%operation%"=="2" goto secret_single
if "%operation%"=="3" goto stego_single
if "%operation%"=="4" goto reference_single
if "%operation%"=="5" goto exit
echo Invalid choice! Press any key to try again...
pause > nul
goto one_file

:two_files
cls
echo =======================================
echo  G-let D3 PGM Steganography - Two Files
echo =======================================
echo.
echo Detected two PGM files:
echo File 1: %FILE1%
echo File 2: %FILE2%
echo.
echo What operation would you like to perform?
echo.
echo 1. Embed (File 1 as cover, File 2 as secret)
echo 2. Embed (File 2 as cover, File 1 as secret)
echo 3. Extract (File 1 as stego image)
echo 4. Extract (File 2 as stego image)
echo 5. Assess quality (File 1 as original, File 2 as modified)
echo 6. Assess quality (File 2 as original, File 1 as modified)
echo 7. Exit
echo.
set /p operation=Enter choice (1-7): 

if "%operation%"=="1" goto embed_1_2
if "%operation%"=="2" goto embed_2_1
if "%operation%"=="3" goto extract_1
if "%operation%"=="4" goto extract_2
if "%operation%"=="5" goto assess_1_2
if "%operation%"=="6" goto assess_2_1
if "%operation%"=="7" goto exit
echo Invalid choice! Press any key to try again...
pause > nul
goto two_files

:cover_single
cls
echo =======================================
echo  Using as Cover Image (to hide a secret)
echo =======================================
echo.
echo Cover image: %FILE1%
echo.
set /p secret=Enter secret image path (.pgm): 
goto process_embed

:secret_single
cls
echo =======================================
echo  Using as Secret Image (to be hidden)
echo =======================================
echo.
echo Secret image: %FILE1%
echo.
set /p cover=Enter cover image path (.pgm): 
set secret=%FILE1%
goto process_embed

:stego_single
cls
echo =======================================
echo  Extract from Stego Image
echo =======================================
echo.
echo Stego image: %FILE1%
echo.
set stego=%FILE1%
goto process_extract

:reference_single
cls
echo =======================================
echo  Using as Reference for Quality Assessment
echo =======================================
echo.
echo Reference image: %FILE1%
echo.
set /p modified=Enter modified/stego image to compare with (.pgm): 
set original=%FILE1%
goto process_assess

:embed_1_2
cls
echo =======================================
echo  Embedding Secret Image
echo =======================================
echo.
echo Cover image: %FILE1%
echo Secret image: %FILE2%
echo.
set cover=%FILE1%
set secret=%FILE2%
goto process_embed

:embed_2_1
cls
echo =======================================
echo  Embedding Secret Image
echo =======================================
echo.
echo Cover image: %FILE2%
echo Secret image: %FILE1%
echo.
set cover=%FILE2%
set secret=%FILE1%
goto process_embed

:extract_1
cls
echo =======================================
echo  Extracting Secret Image
echo =======================================
echo.
echo Stego image: %FILE1%
echo.
set stego=%FILE1%
goto process_extract

:extract_2
cls
echo =======================================
echo  Extracting Secret Image
echo =======================================
echo.
echo Stego image: %FILE2%
echo.
set stego=%FILE2%
goto process_extract

:assess_1_2
cls
echo =======================================
echo  Assessing Image Quality
echo =======================================
echo.
echo Original image: %FILE1%
echo Modified image: %FILE2%
echo.
set original=%FILE1%
set modified=%FILE2%
goto process_assess

:assess_2_1
cls
echo =======================================
echo  Assessing Image Quality
echo =======================================
echo.
echo Original image: %FILE2%
echo Modified image: %FILE1%
echo.
set original=%FILE2%
set modified=%FILE1%
goto process_assess

:process_embed
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
goto exit

:process_extract
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
goto exit

:process_assess
set command=.\bin\stego.exe assess "%original%" "%modified%"

echo.
echo Executing: %command%
echo.
%command%
echo.
echo Operation completed.
pause
goto exit

:usage
cls
echo =======================================
echo  G-let D3 PGM Steganography - Two File Processor
echo =======================================
echo.
echo This tool allows you to process one or two PGM files by dragging and dropping them onto this batch file.
echo.
echo Usage instructions:
echo 1. Drag and drop ONE PGM file to use it for steganography operations
echo 2. Or drag and drop TWO PGM files simultaneously for seamless operations
echo.
echo Examples:
echo - Drag both cover and secret images to embed one into the other
echo - Drag original and stego images to assess quality differences
echo - Drag a stego image to extract the hidden content
echo.
pause
goto exit

:exit
exit /b 