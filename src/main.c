/**
 * main.c
 * Main program for G-let D3 PGM steganography
 */

#include "../include/steganography.h"

void print_usage(const char *program_name) {
    printf("G-let D3 PGM Steganography\n");
    printf("Usage:\n");
    printf("  %s embed <cover_image.pgm> <secret_image.pgm> <output_image.pgm> [options]\n", program_name);
    printf("  %s extract <stego_image.pgm> <output_image.pgm> [width height] [options]\n", program_name);
    printf("  %s assess <original_image.pgm> <modified_image.pgm>\n", program_name);
    printf("\nOptions:\n");
    printf("  embed   - Embed a secret image inside a cover image\n");
    printf("  extract - Extract a secret image from a stego image\n");
    printf("  assess  - Assess the quality difference between two images\n");
    printf("  width   - (Optional) Width of the secret image to extract\n");
    printf("  height  - (Optional) Height of the secret image to extract\n");
    printf("\nAdvanced options (for embed/extract):\n");
    printf("  -b <size>      - Block size (must be power of 2, default: 8)\n");
    printf("  -s <strength>  - Embedding strength (1-10, default: 5)\n");
    printf("  -r             - Use random block selection (increases security)\n");
    printf("  -seed <value>  - Random seed value (default: current time)\n");
}

/**
 * Parse advanced options from command line
 */
void parse_advanced_options(int argc, char *argv[], int start_idx, StegoConfig *config) {
    for (int i = start_idx; i < argc; i++) {
        if (strcmp(argv[i], "-b") == 0 && i + 1 < argc) {
            config->block_size = atoi(argv[i + 1]);
            i++; // Skip the next argument
        }
        else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            config->embedding_strength = atoi(argv[i + 1]);
            
            // Clip the strength to the valid range
            if (config->embedding_strength < 1) config->embedding_strength = 1;
            if (config->embedding_strength > 10) config->embedding_strength = 10;
            
            i++; // Skip the next argument
        }
        else if (strcmp(argv[i], "-r") == 0) {
            config->use_random_blocks = 1;
        }
        else if (strcmp(argv[i], "-seed") == 0 && i + 1 < argc) {
            config->random_seed = atol(argv[i + 1]);
            i++; // Skip the next argument
        }
    }
}

int main(int argc, char *argv[]) {
    // Check command line arguments
    if (argc < 4) {
        print_usage(argv[0]);
        return 1;
    }

    // Parse operation type
    const char *operation = argv[1];

    if (strcmp(operation, "embed") == 0) {
        // Embedding operation
        if (argc < 5) {
            printf("Error: Embedding requires 3 file arguments\n");
            print_usage(argv[0]);
            return 1;
        }

        const char *cover_file = argv[2];
        const char *secret_file = argv[3];
        const char *output_file = argv[4];

        // Create configuration with default values
        StegoConfig config = create_default_config();
        
        // Parse advanced options if provided
        if (argc > 5) {
            parse_advanced_options(argc, argv, 5, &config);
        }

        // Load cover image
        PGMImage *cover = load_pgm(cover_file);
        if (!cover) {
            printf("Error: Failed to load cover image: %s\n", cover_file);
            return 1;
        }

        // Load secret image
        PGMImage *secret = load_pgm(secret_file);
        if (!secret) {
            printf("Error: Failed to load secret image: %s\n", secret_file);
            free_pgm(cover);
            return 1;
        }

        printf("Embedding secret image (%dx%d) into cover image (%dx%d)\n", 
               secret->width, secret->height, cover->width, cover->height);
        printf("Using configuration:\n");
        printf("  Block size: %d\n", config.block_size);
        printf("  Embedding strength: %d\n", config.embedding_strength);
        printf("  Random blocks: %s\n", config.use_random_blocks ? "Yes" : "No");
        if (config.use_random_blocks) {
            printf("  Random seed: %lu\n", config.random_seed);
        }

        // Embed secret image into cover image
        PGMImage *stego = embed_image_with_config(cover, secret, &config);
        if (!stego) {
            printf("Error: Failed to embed secret image\n");
            free_pgm(cover);
            free_pgm(secret);
            return 1;
        }

        // Calculate and display PSNR to assess quality
        double psnr = calculate_psnr(cover, stego);
        double ssim = calculate_ssim(cover, stego);
        printf("PSNR of stego image: %.2f dB (higher is better, >30dB is good)\n", psnr);
        printf("SSIM of stego image: %.4f (closer to 1 is better)\n", ssim);

        // Save stego image
        if (save_pgm(stego, output_file) != 0) {
            printf("Error: Failed to save stego image: %s\n", output_file);
            free_pgm(cover);
            free_pgm(secret);
            free_pgm(stego);
            return 1;
        }

        printf("Success: Secret image embedded and saved to %s\n", output_file);

        // Free memory
        free_pgm(cover);
        free_pgm(secret);
        free_pgm(stego);

    } else if (strcmp(operation, "extract") == 0) {
        // Extraction operation
        if (argc < 4) {
            printf("Error: Extraction requires at least 2 file arguments\n");
            print_usage(argv[0]);
            return 1;
        }

        const char *stego_file = argv[2];
        const char *output_file = argv[3];
        
        int width = 0;
        int height = 0;
        int option_start_idx = 4;

        // Parse optional width/height
        if (argc >= 6 && argv[4][0] != '-' && argv[5][0] != '-') {
            width = atoi(argv[4]);
            height = atoi(argv[5]);
            option_start_idx = 6;
            
            if (width <= 0 || height <= 0) {
                printf("Error: Invalid width/height values\n");
                return 1;
            }
        }

        // Create configuration with default values
        StegoConfig config = create_default_config();
        
        // Parse advanced options if provided
        if (argc > option_start_idx) {
            parse_advanced_options(argc, argv, option_start_idx, &config);
        }

        // Load stego image
        PGMImage *stego = load_pgm(stego_file);
        if (!stego) {
            printf("Error: Failed to load stego image: %s\n", stego_file);
            return 1;
        }

        printf("Extracting secret image from %s\n", stego_file);
        if (width > 0 && height > 0) {
            printf("Using provided dimensions: %dx%d\n", width, height);
        }
        printf("Initial configuration:\n");
        printf("  Block size: %d\n", config.block_size);
        printf("  Embedding strength: %d\n", config.embedding_strength);
        printf("  Random blocks: %s\n", config.use_random_blocks ? "Yes" : "No");
        if (config.use_random_blocks) {
            printf("  Random seed: %lu\n", config.random_seed);
        }

        // Extract secret image
        PGMImage *secret = extract_image_with_config(stego, width, height, &config);
        if (!secret) {
            printf("Error: Failed to extract secret image\n");
            free_pgm(stego);
            return 1;
        }

        printf("Extracted secret image dimensions: %dx%d\n", secret->width, secret->height);

        // Save secret image
        if (save_pgm(secret, output_file) != 0) {
            printf("Error: Failed to save extracted image: %s\n", output_file);
            free_pgm(stego);
            free_pgm(secret);
            return 1;
        }

        printf("Success: Secret image extracted and saved to %s\n", output_file);

        // Free memory
        free_pgm(stego);
        free_pgm(secret);

    } else if (strcmp(operation, "assess") == 0) {
        // Quality assessment operation
        if (argc != 4) {
            printf("Error: Assessment requires 2 file arguments\n");
            print_usage(argv[0]);
            return 1;
        }

        const char *original_file = argv[2];
        const char *modified_file = argv[3];

        // Load original image
        PGMImage *original = load_pgm(original_file);
        if (!original) {
            printf("Error: Failed to load original image: %s\n", original_file);
            return 1;
        }

        // Load modified image
        PGMImage *modified = load_pgm(modified_file);
        if (!modified) {
            printf("Error: Failed to load modified image: %s\n", modified_file);
            free_pgm(original);
            return 1;
        }

        // Calculate and display quality metrics
        double mse = calculate_mse(original, modified);
        double psnr = calculate_psnr(original, modified);
        double ssim = calculate_ssim(original, modified);

        printf("\nQuality Assessment Results:\n");
        printf("-------------------------\n");
        printf("Mean Square Error (MSE): %.4f (lower is better)\n", mse);
        printf("Peak Signal-to-Noise Ratio (PSNR): %.2f dB (higher is better)\n", psnr);
        printf("Structural Similarity Index (SSIM): %.4f (closer to 1 is better)\n", ssim);
        printf("\nInterpretation:\n");
        printf("- PSNR > 30 dB: Good quality\n");
        printf("- PSNR > 40 dB: Excellent quality\n");
        printf("- SSIM > 0.95: High structural similarity\n");
        printf("- SSIM > 0.98: Nearly identical images\n");

        // Free memory
        free_pgm(original);
        free_pgm(modified);

    } else {
        printf("Error: Unknown operation '%s'\n", operation);
        print_usage(argv[0]);
        return 1;
    }

    return 0;
} 