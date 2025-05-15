# G-let D3 PGM Steganography Tool - User Interface Guide

This document explains how to use the user interface options for the G-let D3 PGM steganography program.

## Menu Interface (stego_menu.bat)

A simple menu-driven interface has been created to make the steganography operations more user-friendly.

### Running the Menu

1. Simply double-click on `stego_menu.bat` in Windows Explorer
2. Alternatively, run it from the command line:
   ```
   stego_menu.bat
   ```

### Using the Menu

The menu presents 4 options:

1. **Embed Secret Image** - Hide a secret image inside a cover image
2. **Extract Secret Image** - Extract a hidden image from a stego image
3. **Assess Image Quality** - Compare quality between two images
4. **Exit** - Exit the program

## Drag & Drop Interface (stego_drag_drop.bat)

A drag & drop interface allows you to quickly process PGM files by simply dragging them onto a batch file.

### Using Drag & Drop

1. Locate a PGM image file in Windows Explorer
2. Drag and drop the file onto `stego_drag_drop.bat`
3. Select what operation you want to perform with the file:
   - Use as cover image (to hide a secret image)
   - Use as secret image (to be hidden)
   - Use as stego image (to extract a hidden image)
   - Use as reference for quality assessment
4. Follow the prompts to enter the remaining parameters

### Benefits of Drag & Drop

- Faster workflow for processing multiple files
- No need to type full file paths for the primary file
- Automatically validates that the file is a PGM image
- Intuitive for users familiar with drag & drop operations

## Two-File Processor (stego_two_files.bat)

For maximum efficiency, you can drag and drop TWO files simultaneously onto this batch file.

### Using the Two-File Processor

1. Select TWO PGM files in Windows Explorer (hold Ctrl to select multiple files)
2. Drag and drop both files onto `stego_two_files.bat`
3. The program will detect both files and offer appropriate operations:
   - Embed one file into the other (in either direction)
   - Extract from a stego image
   - Assess quality between the two images
4. Only relevant parameters will be requested based on your selection

### Benefits of the Two-File Processor

- Most efficient workflow for common operations
- Minimizes manual typing of file paths
- Automatically suggests relevant operations based on the files
- Can still be used with a single file if needed

## Parameter Details

### Embedding an Image

When embedding an image, you'll be prompted for:

- **Cover image path** - The original image to hide data in (.pgm file)
- **Secret image path** - The image to hide (.pgm file)
- **Output stego image path** - Where to save the resulting image
- **Block size** - Size of processing blocks (8 is default, must be power of 2)
- **Embedding strength** - How strongly to embed (1-10, higher values reduce quality)
- **Random block selection** - For enhanced security (y/n)
- **Random seed** - If using random blocks, the seed to use

### Extracting an Image

When extracting an image, you'll be prompted for:

- **Stego image path** - The image containing hidden data (.pgm file)
- **Output path** - Where to save the extracted secret image
- **Secret image dimensions** - Width and height (if known)
- **Block size** - Must match the value used during embedding
- **Embedding strength** - Must match the value used during embedding
- **Random block selection** - Whether random blocks were used (y/n)
- **Random seed** - If random blocks were used, the seed that was used

### Assessing Image Quality

When assessing image quality, you'll be prompted for:

- **Original image path** - The reference image (.pgm file)
- **Modified image path** - The image to compare against (.pgm file)

## Tips for Using the Interface

1. For any parameter with a default value, you can press Enter to use the default
2. File paths can be relative or absolute, with or without quotes
3. Keep track of the parameters you use for embedding, as you'll need the same values for extraction
4. For best results, use a strength value of 3-5 for embedding
5. Using random block selection increases security but requires remembering the seed value
6. For batch processing of multiple files, the drag & drop interface is more efficient
7. For common operations like embedding one image into another or comparing two images, use the two-file processor for the fastest workflow

## Which Interface to Use?

- **Menu Interface**: Best for beginners or when you're not sure what operation to perform
- **Drag & Drop**: Best when working with a single file at a time and you know what operation to perform
- **Two-File Processor**: Most efficient for operations that involve two files, like embedding or quality assessment

## Important Notes

- The user interfaces are wrappers around the command-line tool and require the base program (`bin/stego.exe`) to work
- All operations will show their output in the console window
- Press any key to return to the main menu after an operation finishes 