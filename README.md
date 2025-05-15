# G-let D3 PGM Steganography

A C program for embedding one PGM image inside another using G-let D3 wavelet transform-based steganography techniques.

## Features

- Hide one PGM image (secret) inside another PGM image (cover)
- Extract hidden images from stego images
- Preserves quality of the cover image
- Uses G-let D3 wavelet transform for improved steganography
- Configurable embedding parameters for quality/security trade-offs
- Quality assessment tools for steganographic images
- Command-line interface for easy integration into scripts

## Requirements

- C compiler (GCC recommended)
- Make build system
- Math library (libm)

## Building

To build the program, run:

```bash
make
```

This will create the executable `bin/stego`.

## Usage

### Embedding an Image

To hide a secret image inside a cover image:

```bash
./bin/stego embed <cover_image.pgm> <secret_image.pgm> <output_image.pgm> [options]
```

Where:
- `<cover_image.pgm>` is the original image that will hide the secret
- `<secret_image.pgm>` is the image to be hidden
- `<output_image.pgm>` is where the resulting stego image will be saved

### Extracting an Image

To extract a hidden image from a stego image:

```bash
./bin/stego extract <stego_image.pgm> <output_image.pgm> [width height] [options]
```

Where:
- `<stego_image.pgm>` is the image containing the hidden data
- `<output_image.pgm>` is where the extracted secret image will be saved
- `[width height]` are optional dimensions of the secret image (if known)

If width and height are not provided, the program will try to extract them from the stego image.

### Assessing Image Quality

To compare the quality between two images (e.g., cover and stego):

```bash
./bin/stego assess <original_image.pgm> <modified_image.pgm>
```

This will calculate and display:
- Mean Square Error (MSE)
- Peak Signal-to-Noise Ratio (PSNR)
- Structural Similarity Index (SSIM)

### Advanced Options

The program supports several advanced options for both embedding and extraction:

```
-b <size>      - Block size (must be power of 2, default: 8)
-s <strength>  - Embedding strength (1-10, default: 5)
-r             - Use random block selection (increases security)
-seed <value>  - Random seed value (default: current time)
```

Examples:

```bash
# Embed with higher quality (lower strength)
./bin/stego embed cover.pgm secret.pgm stego.pgm -s 3

# Embed with higher security using random blocks
./bin/stego embed cover.pgm secret.pgm stego.pgm -r -seed 12345

# Extract using the same random block pattern
./bin/stego extract stego.pgm extracted.pgm -r -seed 12345
```

## PGM Image Format

This program works with the PGM (Portable Gray Map) image format, specifically the P5 (binary) variant. You can convert images to PGM format using tools like ImageMagick:

```bash
convert input.png -compress none output.pgm
```

## Implementation Details

The steganography technique uses the G-let D3 wavelet transform to hide data in the high-frequency coefficients of the cover image, which are less perceptible to the human eye. 

The process involves:
1. Dividing the cover image into blocks
2. Applying the G-let D3 transform to each block
3. Embedding secret image data in the high-frequency coefficients
4. Applying the inverse G-let D3 transform to obtain the stego image

### Quality vs. Security Trade-offs

The program allows for adjusting several parameters to balance security and image quality:

- **Block size**: Larger blocks preserve more image quality but reduce capacity
- **Embedding strength**: Higher values make the hidden data more robust but reduce visual quality
- **Random block selection**: Increases security by using a pseudo-random pattern for embedding

### Quality Metrics

- **PSNR (Peak Signal-to-Noise Ratio)**: Values above 30dB indicate good quality, above 40dB excellent quality
- **SSIM (Structural Similarity Index)**: Values closer to 1.0 indicate higher similarity between original and stego images

## Generating Test Images

A utility program is included to generate test PGM images for experimentation:

```bash
./bin/pgm_generator gradient image.pgm 512 512
./bin/pgm_generator checkerboard image.pgm 512 512 32
./bin/pgm_generator noise image.pgm 512 512
./bin/pgm_generator text image.pgm 512 512 "SECRET"
```

## License

This software is provided "as is" without warranty of any kind.

## Author

This program was created for steganography using G-let D3 techniques. 