/**
 * pgm.c
 * Implementation of PGM image handling functions
 */

#include "../include/steganography.h"

/**
 * Load a PGM image from a file
 */
PGMImage* load_pgm(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file %s\n", filename);
        return NULL;
    }

    // Allocate memory for the image
    PGMImage *img = (PGMImage *)malloc(sizeof(PGMImage));
    if (!img) {
        fclose(file);
        return NULL;
    }

    // Read magic number
    char magic[3];
    if (fscanf(file, "%2s", magic) != 1) {
        fprintf(stderr, "Error: Failed to read magic number\n");
        fclose(file);
        free(img);
        return NULL;
    }

    if (strcmp(magic, "P5") != 0) {
        fprintf(stderr, "Error: Not a valid PGM file (P5 format expected)\n");
        fclose(file);
        free(img);
        return NULL;
    }

    // Skip comments and whitespace
    int c;
    while ((c = fgetc(file)) != EOF && (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '#')) {
        if (c == '#') {
            // Skip the entire comment line
            while ((c = fgetc(file)) != EOF && c != '\n');
        }
    }
    
    // Put back the last non-whitespace/non-comment character
    ungetc(c, file);

    // Read width and height
    if (fscanf(file, "%d %d", &img->width, &img->height) != 2) {
        fprintf(stderr, "Error: Failed to read width and height\n");
        fclose(file);
        free(img);
        return NULL;
    }

    printf("Read image dimensions: %dx%d\n", img->width, img->height);

    // Skip any whitespace
    while ((c = fgetc(file)) != EOF && (c == ' ' || c == '\t' || c == '\n' || c == '\r'));
    ungetc(c, file);

    // Read max gray value
    if (fscanf(file, "%d", &img->max_gray) != 1) {
        fprintf(stderr, "Error: Failed to read max gray value\n");
        fclose(file);
        free(img);
        return NULL;
    }

    printf("Read max gray value: %d\n", img->max_gray);
    
    // Skip whitespace until we reach the binary data
    // There should be exactly one whitespace character (newline, space, etc.) after max_gray value
    c = fgetc(file);
    if (c == EOF || !(c == ' ' || c == '\t' || c == '\n' || c == '\r')) {
        fprintf(stderr, "Error: Missing whitespace after max gray value\n");
        fclose(file);
        free(img);
        return NULL;
    }

    // Allocate memory for the image data
    img->data = (unsigned char *)malloc(img->width * img->height * sizeof(unsigned char));
    if (!img->data) {
        fprintf(stderr, "Error: Failed to allocate memory for image data\n");
        fclose(file);
        free(img);
        return NULL;
    }

    // Read the image data
    size_t bytes_read = fread(img->data, sizeof(unsigned char), img->width * img->height, file);
    if (bytes_read != img->width * img->height) {
        fprintf(stderr, "Error: Failed to read image data. Expected %d bytes, got %zu bytes\n", 
                img->width * img->height, bytes_read);
        fclose(file);
        free(img->data);
        free(img);
        return NULL;
    }

    fclose(file);
    printf("Successfully loaded PGM image: %s (%dx%d)\n", filename, img->width, img->height);
    return img;
}

/**
 * Save a PGM image to a file
 */
int save_pgm(PGMImage *img, const char *filename) {
    if (!img || !img->data) {
        fprintf(stderr, "Error: Invalid image data\n");
        return -1;
    }

    FILE *file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file %s for writing\n", filename);
        return -1;
    }

    // Write header
    fprintf(file, "P5\n");
    fprintf(file, "# Created by G-let D3 Steganography\n");
    fprintf(file, "%d %d\n", img->width, img->height);
    fprintf(file, "%d\n", img->max_gray);

    // Write image data
    size_t bytes_written = fwrite(img->data, sizeof(unsigned char), img->width * img->height, file);
    if (bytes_written != img->width * img->height) {
        fprintf(stderr, "Error: Failed to write image data. Expected %d bytes, wrote %zu bytes\n", 
                img->width * img->height, bytes_written);
        fclose(file);
        return -1;
    }

    fclose(file);
    printf("Successfully saved PGM image: %s (%dx%d)\n", filename, img->width, img->height);
    return 0;
}

/**
 * Free memory allocated for a PGM image
 */
void free_pgm(PGMImage *img) {
    if (img) {
        if (img->data) {
            free(img->data);
        }
        free(img);
    }
} 