/**
 * pgm_generator.c
 * Utility to generate test PGM images
 * Compile separately: gcc -o pgm_generator pgm_generator.c -lm
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

void generate_gradient(const char *filename, int width, int height) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file %s for writing\n", filename);
        return;
    }

    // Write header
    fprintf(file, "P5\n");
    fprintf(file, "# PGM Gradient Test Image\n");
    fprintf(file, "%d %d\n", width, height);
    fprintf(file, "255\n");

    // Write image data - horizontal gradient
    unsigned char *data = (unsigned char *)malloc(width * height);
    if (!data) {
        fclose(file);
        return;
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Create a gradient from 0 to 255
            data[y * width + x] = (unsigned char)(255.0 * x / width);
        }
    }

    fwrite(data, 1, width * height, file);
    fclose(file);
    free(data);
    printf("Generated gradient image: %s (%dx%d)\n", filename, width, height);
}

void generate_checkerboard(const char *filename, int width, int height, int square_size) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file %s for writing\n", filename);
        return;
    }

    // Write header
    fprintf(file, "P5\n");
    fprintf(file, "# PGM Checkerboard Test Image\n");
    fprintf(file, "%d %d\n", width, height);
    fprintf(file, "255\n");

    // Write image data - checkerboard pattern
    unsigned char *data = (unsigned char *)malloc(width * height);
    if (!data) {
        fclose(file);
        return;
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Create a checkerboard pattern
            int square_x = x / square_size;
            int square_y = y / square_size;
            if ((square_x + square_y) % 2 == 0) {
                data[y * width + x] = 0;    // Black
            } else {
                data[y * width + x] = 255;  // White
            }
        }
    }

    fwrite(data, 1, width * height, file);
    fclose(file);
    free(data);
    printf("Generated checkerboard image: %s (%dx%d, square size: %d)\n", 
           filename, width, height, square_size);
}

void generate_noise(const char *filename, int width, int height) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file %s for writing\n", filename);
        return;
    }

    // Seed random number generator
    srand(time(NULL));

    // Write header
    fprintf(file, "P5\n");
    fprintf(file, "# PGM Random Noise Test Image\n");
    fprintf(file, "%d %d\n", width, height);
    fprintf(file, "255\n");

    // Write image data - random noise
    unsigned char *data = (unsigned char *)malloc(width * height);
    if (!data) {
        fclose(file);
        return;
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Generate random grayscale value
            data[y * width + x] = (unsigned char)(rand() % 256);
        }
    }

    fwrite(data, 1, width * height, file);
    fclose(file);
    free(data);
    printf("Generated noise image: %s (%dx%d)\n", filename, width, height);
}

void generate_text(const char *filename, int width, int height, const char *text) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file %s for writing\n", filename);
        return;
    }

    // Write header
    fprintf(file, "P5\n");
    fprintf(file, "# PGM Text Test Image\n");
    fprintf(file, "%d %d\n", width, height);
    fprintf(file, "255\n");

    // Write image data - white background
    unsigned char *data = (unsigned char *)malloc(width * height);
    if (!data) {
        fclose(file);
        return;
    }
    
    // Initialize with white background
    memset(data, 255, width * height);

    // A very simple "font" - just draw the text as black pixels
    int text_len = strlen(text);
    int char_width = 8;
    int char_height = 12;
    int start_x = (width - text_len * char_width) / 2;
    int start_y = (height - char_height) / 2;

    if (start_x < 0 || start_y < 0) {
        fprintf(stderr, "Error: Image too small for text\n");
        fclose(file);
        free(data);
        return;
    }

    for (int i = 0; i < text_len; i++) {
        int char_start_x = start_x + i * char_width;
        char c = text[i];
        
        // Very simple font rendering - just draw the character outline
        switch (c) {
            case 'A':
                for (int y = 0; y < char_height; y++) {
                    for (int x = 0; x < char_width; x++) {
                        // Draw an 'A' shape
                        if ((x == 0 || x == char_width-1) && y >= char_height/2 ||
                            y == char_height/2 ||
                            (y == 0 && x >= char_width/3 && x <= 2*char_width/3)) {
                            if (char_start_x + x < width && start_y + y < height) {
                                data[(start_y + y) * width + (char_start_x + x)] = 0;
                            }
                        }
                    }
                }
                break;
            default:
                // For simplicity, draw a box for any other character
                for (int y = 0; y < char_height; y++) {
                    for (int x = 0; x < char_width; x++) {
                        if (y == 0 || y == char_height-1 || x == 0 || x == char_width-1) {
                            if (char_start_x + x < width && start_y + y < height) {
                                data[(start_y + y) * width + (char_start_x + x)] = 0;
                            }
                        }
                    }
                }
                break;
        }
    }

    fwrite(data, 1, width * height, file);
    fclose(file);
    free(data);
    printf("Generated text image: %s (%dx%d, text: \"%s\")\n", 
           filename, width, height, text);
}

void print_usage(const char *program_name) {
    printf("PGM Test Image Generator\n");
    printf("Usage:\n");
    printf("  %s gradient <filename.pgm> <width> <height>\n", program_name);
    printf("  %s checkerboard <filename.pgm> <width> <height> <square_size>\n", program_name);
    printf("  %s noise <filename.pgm> <width> <height>\n", program_name);
    printf("  %s text <filename.pgm> <width> <height> <text>\n", program_name);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    const char *operation = argv[1];

    if (strcmp(operation, "gradient") == 0) {
        if (argc != 5) {
            printf("Error: Gradient generator requires filename, width, and height\n");
            print_usage(argv[0]);
            return 1;
        }
        const char *filename = argv[2];
        int width = atoi(argv[3]);
        int height = atoi(argv[4]);
        
        if (width <= 0 || height <= 0) {
            printf("Error: Invalid dimensions\n");
            return 1;
        }
        
        generate_gradient(filename, width, height);
    }
    else if (strcmp(operation, "checkerboard") == 0) {
        if (argc != 6) {
            printf("Error: Checkerboard generator requires filename, width, height, and square size\n");
            print_usage(argv[0]);
            return 1;
        }
        const char *filename = argv[2];
        int width = atoi(argv[3]);
        int height = atoi(argv[4]);
        int square_size = atoi(argv[5]);
        
        if (width <= 0 || height <= 0 || square_size <= 0) {
            printf("Error: Invalid dimensions or square size\n");
            return 1;
        }
        
        generate_checkerboard(filename, width, height, square_size);
    }
    else if (strcmp(operation, "noise") == 0) {
        if (argc != 5) {
            printf("Error: Noise generator requires filename, width, and height\n");
            print_usage(argv[0]);
            return 1;
        }
        const char *filename = argv[2];
        int width = atoi(argv[3]);
        int height = atoi(argv[4]);
        
        if (width <= 0 || height <= 0) {
            printf("Error: Invalid dimensions\n");
            return 1;
        }
        
        generate_noise(filename, width, height);
    }
    else if (strcmp(operation, "text") == 0) {
        if (argc != 6) {
            printf("Error: Text generator requires filename, width, height, and text\n");
            print_usage(argv[0]);
            return 1;
        }
        const char *filename = argv[2];
        int width = atoi(argv[3]);
        int height = atoi(argv[4]);
        const char *text = argv[5];
        
        if (width <= 0 || height <= 0) {
            printf("Error: Invalid dimensions\n");
            return 1;
        }
        
        generate_text(filename, width, height, text);
    }
    else {
        printf("Error: Unknown operation '%s'\n", operation);
        print_usage(argv[0]);
        return 1;
    }

    return 0;
} 