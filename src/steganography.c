/**
 * steganography.c
 * Implementation of steganography functions using G-let D3 wavelet transform
 */

#include "../include/steganography.h"

/**
 * Round a double value to the nearest integer and clip to [0, 255]
 */
static int clip_to_byte(double val) {
    int rounded = (int)(val + 0.5);
    if (rounded < 0) return 0;
    if (rounded > 255) return 255;
    return rounded;
}

/**
 * Check if a number is a power of 2
 */
static int is_power_of_two(int n) {
    return (n != 0) && ((n & (n - 1)) == 0);
}

/**
 * Get the next power of 2 greater than or equal to n
 */
static int next_power_of_two(int n) {
    if (n <= 0) return 1;
    if (is_power_of_two(n)) return n;
    
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;
    
    return n;
}

/**
 * Create default steganography configuration
 */
StegoConfig create_default_config() {
    StegoConfig config;
    config.block_size = 8;              // 8x8 blocks
    config.embedding_strength = 5;      // Medium embedding strength
    config.use_random_blocks = 0;       // No randomization by default
    config.random_seed = time(NULL);    // Current time as seed
    return config;
}

/**
 * Embed a secret PGM image into a cover PGM image using default configuration
 */
PGMImage* embed_image(PGMImage *cover, PGMImage *secret) {
    StegoConfig config = create_default_config();
    return embed_image_with_config(cover, secret, &config);
}

/**
 * Extract a secret PGM image from a stego image using default configuration
 */
PGMImage* extract_image(PGMImage *stego, int width, int height) {
    StegoConfig config = create_default_config();
    return extract_image_with_config(stego, width, height, &config);
}

/**
 * Embed a secret PGM image into a cover PGM image using G-let D3 steganography
 */
PGMImage* embed_image_with_config(PGMImage *cover, PGMImage *secret, StegoConfig *config) {
    if (!cover || !secret || !cover->data || !secret->data) {
        fprintf(stderr, "Error: Invalid input images\n");
        return NULL;
    }

    // Use default config if none provided
    StegoConfig default_config;
    if (!config) {
        default_config = create_default_config();
        config = &default_config;
    }

    // Verify that the secret image can fit in the cover image
    if (cover->width < secret->width || cover->height < secret->height) {
        fprintf(stderr, "Error: Secret image is larger than cover image\n");
        return NULL;
    }

    // Create a copy of the cover image
    PGMImage *stego = (PGMImage *)malloc(sizeof(PGMImage));
    if (!stego) return NULL;

    stego->width = cover->width;
    stego->height = cover->height;
    stego->max_gray = cover->max_gray;
    stego->data = (unsigned char *)malloc(stego->width * stego->height * sizeof(unsigned char));
    
    if (!stego->data) {
        free(stego);
        return NULL;
    }

    // Copy cover image data
    memcpy(stego->data, cover->data, stego->width * stego->height * sizeof(unsigned char));

    // Determine block size (next power of 2)
    int block_size = is_power_of_two(config->block_size) ? 
                     config->block_size : next_power_of_two(config->block_size);

    // Calculate the number of blocks in the cover image
    int blocks_x = stego->width / block_size;
    int blocks_y = stego->height / block_size;

    // Set random seed if using random blocks
    if (config->use_random_blocks) {
        srand(config->random_seed);
    }

    // Write secret image dimensions and config in the first block (for extraction later)
    // We'll use the high-frequency coefficients of the first block
    double *first_block = (double *)malloc(block_size * block_size * sizeof(double));
    if (!first_block) {
        free(stego->data);
        free(stego);
        return NULL;
    }

    // Copy first block data to double array
    for (int i = 0; i < block_size; i++) {
        for (int j = 0; j < block_size; j++) {
            if (i < stego->height && j < stego->width) {
                first_block[i * block_size + j] = stego->data[i * stego->width + j];
            } else {
                first_block[i * block_size + j] = 0;
            }
        }
    }

    // Apply forward G-let D3 transform
    glet_d3_forward(first_block, block_size);

    // Embed secret image dimensions and config in high-frequency coefficients
    // We'll use positions that won't visibly affect the image
    first_block[block_size * (block_size / 2) + block_size / 2 + 1] = secret->width;
    first_block[block_size * (block_size / 2 + 1) + block_size / 2] = secret->height;
    first_block[block_size * (block_size / 2 + 2) + block_size / 2] = config->block_size;
    first_block[block_size * (block_size / 2 + 3) + block_size / 2] = config->embedding_strength;
    first_block[block_size * (block_size / 2 + 4) + block_size / 2] = config->use_random_blocks;

    // Apply inverse G-let D3 transform
    glet_d3_inverse(first_block, block_size);

    // Copy modified first block back to stego image
    for (int i = 0; i < block_size && i < stego->height; i++) {
        for (int j = 0; j < block_size && j < stego->width; j++) {
            stego->data[i * stego->width + j] = clip_to_byte(first_block[i * block_size + j]);
        }
    }

    free(first_block);

    // Process each block (skip the first block which contains metadata)
    double *cover_block = (double *)malloc(block_size * block_size * sizeof(double));
    if (!cover_block) {
        free(stego->data);
        free(stego);
        return NULL;
    }

    // Calculate the embedding factor based on strength (1-10)
    double embedding_factor = config->embedding_strength / 10.0;
    
    // Pre-calculate or store random block sequence if using randomization
    int *block_sequence = NULL;
    int total_blocks = blocks_x * blocks_y - 1; // minus the first metadata block
    
    if (config->use_random_blocks) {
        // Allocate and initialize sequence
        block_sequence = (int *)malloc(total_blocks * sizeof(int));
        if (!block_sequence) {
            free(cover_block);
            free(stego->data);
            free(stego);
            return NULL;
        }
        
        // Initialize with sequential order
        for (int i = 0; i < total_blocks; i++) {
            block_sequence[i] = i + 1; // +1 to skip first block
        }
        
        // Fisher-Yates shuffle
        for (int i = total_blocks - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            // Swap
            int temp = block_sequence[i];
            block_sequence[i] = block_sequence[j];
            block_sequence[j] = temp;
        }
    }

    // Iterate through secret image pixels and embed them
    int secret_pixels = secret->width * secret->height;
    int embedded_count = 0;
    
    while (embedded_count < secret_pixels && embedded_count < total_blocks) {
        // Determine which block to use
        int block_idx;
        
        if (config->use_random_blocks) {
            block_idx = block_sequence[embedded_count]; 
        } else {
            block_idx = embedded_count + 1; // +1 to skip first block
        }
        
        // Convert linear block index to 2D coordinates
        int bx = block_idx % blocks_x;
        int by = block_idx / blocks_x;
        
        // Calculate secret image pixel to embed
        int secret_x = embedded_count % secret->width;
        int secret_y = embedded_count / secret->width;
        unsigned char pixel = secret->data[secret_y * secret->width + secret_x];
        
        // Copy current block data to double array
        for (int i = 0; i < block_size; i++) {
            for (int j = 0; j < block_size; j++) {
                int cover_y = by * block_size + i;
                int cover_x = bx * block_size + j;
                
                if (cover_y < stego->height && cover_x < stego->width) {
                    cover_block[i * block_size + j] = stego->data[cover_y * stego->width + cover_x];
                } else {
                    cover_block[i * block_size + j] = 0;
                }
            }
        }

        // Apply forward G-let D3 transform
        glet_d3_forward(cover_block, block_size);

        // Embed one pixel of secret image in high-frequency coefficients
        // Embed 8 bits of the pixel into 8 different high-frequency coefficients
        for (int bit = 0; bit < 8; bit++) {
            // Select a high-frequency coefficient position (avoid low frequencies)
            int coef_y = block_size / 2 + bit % 4;
            int coef_x = block_size / 2 + bit / 4;
            
            // Extract the bit to embed
            int pixel_bit = (pixel >> bit) & 1;
            
            // Modify the coefficient based on the bit and embedding factor
            double coef_val = cover_block[coef_y * block_size + coef_x];
            if (pixel_bit == 1) {
                // For 1, make coefficient slightly more positive
                cover_block[coef_y * block_size + coef_x] = coef_val + embedding_factor;
            } else {
                // For 0, make coefficient slightly more negative
                cover_block[coef_y * block_size + coef_x] = coef_val - embedding_factor;
            }
        }

        // Apply inverse G-let D3 transform
        glet_d3_inverse(cover_block, block_size);

        // Copy modified block back to stego image
        for (int i = 0; i < block_size; i++) {
            for (int j = 0; j < block_size; j++) {
                int cover_y = by * block_size + i;
                int cover_x = bx * block_size + j;
                
                if (cover_y < stego->height && cover_x < stego->width) {
                    stego->data[cover_y * stego->width + cover_x] = clip_to_byte(cover_block[i * block_size + j]);
                }
            }
        }
        
        embedded_count++;
    }

    // Free allocated memory
    free(cover_block);
    if (block_sequence) {
        free(block_sequence);
    }
    
    return stego;
}

/**
 * Extract a secret PGM image from a stego image
 */
PGMImage* extract_image_with_config(PGMImage *stego, int width, int height, StegoConfig *config) {
    if (!stego || !stego->data) {
        fprintf(stderr, "Error: Invalid stego image\n");
        return NULL;
    }

    // Use default config if none provided
    StegoConfig default_config;
    if (!config) {
        default_config = create_default_config();
        config = &default_config;
    }

    // Determine block size from the stego image if not specified
    int block_size = is_power_of_two(config->block_size) ? 
                     config->block_size : next_power_of_two(config->block_size);

    // If dimensions and config not provided, extract them from the first block
    if (width <= 0 || height <= 0) {
        double *first_block = (double *)malloc(block_size * block_size * sizeof(double));
        if (!first_block) return NULL;

        // Copy first block data to double array
        for (int i = 0; i < block_size && i < stego->height; i++) {
            for (int j = 0; j < block_size && j < stego->width; j++) {
                first_block[i * block_size + j] = stego->data[i * stego->width + j];
            }
        }

        // Apply forward G-let D3 transform
        glet_d3_forward(first_block, block_size);

        // Extract secret image dimensions and configuration from high-frequency coefficients
        width = (int)first_block[block_size * (block_size / 2) + block_size / 2 + 1];
        height = (int)first_block[block_size * (block_size / 2 + 1) + block_size / 2];
        config->block_size = (int)first_block[block_size * (block_size / 2 + 2) + block_size / 2];
        config->embedding_strength = (int)first_block[block_size * (block_size / 2 + 3) + block_size / 2];
        config->use_random_blocks = (int)first_block[block_size * (block_size / 2 + 4) + block_size / 2];

        free(first_block);
        
        // Update block size from extracted config
        block_size = is_power_of_two(config->block_size) ? 
                     config->block_size : next_power_of_two(config->block_size);
    }

    // Verify extracted dimensions
    if (width <= 0 || height <= 0 || width > stego->width || height > stego->height) {
        fprintf(stderr, "Error: Invalid secret image dimensions extracted: %dx%d\n", width, height);
        return NULL;
    }

    printf("Detected steganography configuration:\n");
    printf("Block size: %d\n", config->block_size);
    printf("Embedding strength: %d\n", config->embedding_strength);
    printf("Using random blocks: %s\n", config->use_random_blocks ? "Yes" : "No");

    // Create the secret image
    PGMImage *secret = (PGMImage *)malloc(sizeof(PGMImage));
    if (!secret) return NULL;

    secret->width = width;
    secret->height = height;
    secret->max_gray = stego->max_gray;
    secret->data = (unsigned char *)malloc(secret->width * secret->height * sizeof(unsigned char));
    
    if (!secret->data) {
        free(secret);
        return NULL;
    }

    // Initialize the secret image data to zeros
    memset(secret->data, 0, secret->width * secret->height * sizeof(unsigned char));

    // Calculate the number of blocks in the stego image
    int blocks_x = stego->width / block_size;
    int blocks_y = stego->height / block_size;

    // Set random seed if using random blocks
    if (config->use_random_blocks) {
        srand(config->random_seed);
    }

    // Pre-calculate or store random block sequence if using randomization
    int *block_sequence = NULL;
    int total_blocks = blocks_x * blocks_y - 1; // minus the first metadata block
    
    if (config->use_random_blocks) {
        // Allocate and initialize sequence
        block_sequence = (int *)malloc(total_blocks * sizeof(int));
        if (!block_sequence) {
            free(secret->data);
            free(secret);
            return NULL;
        }
        
        // Initialize with sequential order
        for (int i = 0; i < total_blocks; i++) {
            block_sequence[i] = i + 1; // +1 to skip first block
        }
        
        // Fisher-Yates shuffle with same seed used during embedding
        for (int i = total_blocks - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            // Swap
            int temp = block_sequence[i];
            block_sequence[i] = block_sequence[j];
            block_sequence[j] = temp;
        }
    }

    // Process each block (skip the first block which contains metadata)
    double *stego_block = (double *)malloc(block_size * block_size * sizeof(double));
    if (!stego_block) {
        if (block_sequence) free(block_sequence);
        free(secret->data);
        free(secret);
        return NULL;
    }

    // Calculate the embedding factor based on strength (1-10)
    double embedding_factor = config->embedding_strength / 10.0;

    // Iterate through secret image pixels and extract them
    int secret_pixels = secret->width * secret->height;
    int extracted_count = 0;
    
    while (extracted_count < secret_pixels && extracted_count < total_blocks) {
        // Determine which block to use
        int block_idx;
        
        if (config->use_random_blocks) {
            block_idx = block_sequence[extracted_count]; 
        } else {
            block_idx = extracted_count + 1; // +1 to skip first block
        }
        
        // Convert linear block index to 2D coordinates
        int bx = block_idx % blocks_x;
        int by = block_idx / blocks_x;
        
        // Calculate secret image pixel position
        int secret_x = extracted_count % secret->width;
        int secret_y = extracted_count / secret->width;
        
        // Copy current block data to double array
        for (int i = 0; i < block_size; i++) {
            for (int j = 0; j < block_size; j++) {
                int stego_y = by * block_size + i;
                int stego_x = bx * block_size + j;
                
                if (stego_y < stego->height && stego_x < stego->width) {
                    stego_block[i * block_size + j] = stego->data[stego_y * stego->width + stego_x];
                } else {
                    stego_block[i * block_size + j] = 0;
                }
            }
        }

        // Apply forward G-let D3 transform
        glet_d3_forward(stego_block, block_size);

        // Extract one pixel of secret image from high-frequency coefficients
        unsigned char pixel = 0;
        
        // Extract 8 bits of the pixel from 8 different high-frequency coefficients
        for (int bit = 0; bit < 8; bit++) {
            // Select the same high-frequency coefficient position used during embedding
            int coef_y = block_size / 2 + bit % 4;
            int coef_x = block_size / 2 + bit / 4;
            
            // Extract the bit based on the sign of the coefficient
            double coef_val = stego_block[coef_y * block_size + coef_x];
            int pixel_bit = (coef_val >= 0) ? 1 : 0;
            
            // Add the bit to the pixel
            pixel |= (pixel_bit << bit);
        }
        
        // Store the extracted pixel
        secret->data[secret_y * secret->width + secret_x] = pixel;
        
        extracted_count++;
    }

    // Free allocated memory
    free(stego_block);
    if (block_sequence) {
        free(block_sequence);
    }
    
    return secret;
} 