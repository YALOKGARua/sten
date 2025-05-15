/**
 * steganography.h
 * Header file for G-let D3 based PGM steganography
 */

#ifndef STEGANOGRAPHY_H
#define STEGANOGRAPHY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

/**
 * Structure to represent a PGM image
 */
typedef struct {
    int width;          // Width of the image
    int height;         // Height of the image
    int max_gray;       // Maximum gray value
    unsigned char *data; // Image data
} PGMImage;

/**
 * Configuration for steganography operations
 */
typedef struct {
    int block_size;             // Block size for G-let D3 transform (power of 2)
    int embedding_strength;     // Embedding strength (1-10, higher means stronger embedding but lower quality)
    int use_random_blocks;      // Whether to use random blocks for embedding (increases security)
    unsigned long random_seed;  // Seed for random block selection
} StegoConfig;

/**
 * Create default steganography configuration
 * @return Default configuration
 */
StegoConfig create_default_config();

/**
 * Load a PGM image from a file
 * @param filename Path to the PGM file
 * @return Loaded PGM image structure or NULL on failure
 */
PGMImage* load_pgm(const char *filename);

/**
 * Save a PGM image to a file
 * @param img PGM image to save
 * @param filename Path where to save the image
 * @return 0 on success, -1 on failure
 */
int save_pgm(PGMImage *img, const char *filename);

/**
 * Free memory allocated for a PGM image
 * @param img PGM image to free
 */
void free_pgm(PGMImage *img);

/**
 * Embed a secret PGM image into a cover PGM image using G-let D3 steganography
 * @param cover Cover image where the secret will be hidden
 * @param secret Secret image to hide
 * @param config Steganography configuration (or NULL for default)
 * @return New PGM image with embedded data or NULL on failure
 */
PGMImage* embed_image_with_config(PGMImage *cover, PGMImage *secret, StegoConfig *config);

/**
 * Embed a secret PGM image into a cover PGM image using default configuration
 * @param cover Cover image where the secret will be hidden
 * @param secret Secret image to hide
 * @return New PGM image with embedded data or NULL on failure
 */
PGMImage* embed_image(PGMImage *cover, PGMImage *secret);

/**
 * Extract a secret PGM image from a stego image
 * @param stego Image containing the hidden data
 * @param width Width of the secret image (if known, 0 otherwise)
 * @param height Height of the secret image (if known, 0 otherwise)
 * @param config Steganography configuration (or NULL for default)
 * @return Extracted secret image or NULL on failure
 */
PGMImage* extract_image_with_config(PGMImage *stego, int width, int height, StegoConfig *config);

/**
 * Extract a secret PGM image from a stego image using default configuration
 * @param stego Image containing the hidden data
 * @param width Width of the secret image (if known, 0 otherwise)
 * @param height Height of the secret image (if known, 0 otherwise)
 * @return Extracted secret image or NULL on failure
 */
PGMImage* extract_image(PGMImage *stego, int width, int height);

/**
 * Apply G-let D3 forward transform to an image block
 * @param block Image block data
 * @param size Block size (must be a power of 2)
 */
void glet_d3_forward(double *block, int size);

/**
 * Apply G-let D3 inverse transform to an image block
 * @param block Image block data
 * @param size Block size (must be a power of 2)
 */
void glet_d3_inverse(double *block, int size);

/**
 * Calculate the Peak Signal-to-Noise Ratio between two images
 * @param original Original image
 * @param modified Modified image
 * @return PSNR value in decibels (dB)
 */
double calculate_psnr(PGMImage *original, PGMImage *modified);

/**
 * Calculate the Mean Square Error between two images
 * @param img1 First image
 * @param img2 Second image
 * @return MSE value
 */
double calculate_mse(PGMImage *img1, PGMImage *img2);

/**
 * Calculate the Structural Similarity Index (SSIM) between two images
 * @param img1 First image
 * @param img2 Second image
 * @return SSIM value (between -1 and 1, where 1 means identical images)
 */
double calculate_ssim(PGMImage *img1, PGMImage *img2);

#endif /* STEGANOGRAPHY_H */ 