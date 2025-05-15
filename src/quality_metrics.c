/**
 * quality_metrics.c
 * Implementation of image quality metrics for steganography
 */

#include "../include/steganography.h"

/**
 * Calculate the Mean Square Error between two images
 */
double calculate_mse(PGMImage *img1, PGMImage *img2) {
    if (!img1 || !img2 || !img1->data || !img2->data) {
        fprintf(stderr, "Error: Invalid images for MSE calculation\n");
        return -1.0;
    }

    // Check if dimensions match
    if (img1->width != img2->width || img1->height != img2->height) {
        fprintf(stderr, "Error: Image dimensions do not match for MSE calculation\n");
        return -1.0;
    }

    double sum = 0.0;
    int pixel_count = img1->width * img1->height;

    // Calculate sum of squared differences
    for (int i = 0; i < pixel_count; i++) {
        double diff = (double)img1->data[i] - (double)img2->data[i];
        sum += diff * diff;
    }

    // Calculate mean
    double mse = sum / pixel_count;
    return mse;
}

/**
 * Calculate the Peak Signal-to-Noise Ratio between two images
 */
double calculate_psnr(PGMImage *original, PGMImage *modified) {
    double mse = calculate_mse(original, modified);
    if (mse <= 0.0) {
        return 100.0; // Return a high value for identical images (MSE=0)
    }

    // PSNR calculation
    double max_value = original->max_gray;
    double psnr = 10.0 * log10((max_value * max_value) / mse);
    return psnr;
}

/**
 * Calculate the mean of a window in the image
 */
static double calculate_window_mean(unsigned char *data, int width, int window_x, int window_y, int window_size) {
    double sum = 0.0;
    int count = 0;

    for (int y = 0; y < window_size; y++) {
        for (int x = 0; x < window_size; x++) {
            int pos_x = window_x + x;
            int pos_y = window_y + y;
            sum += data[pos_y * width + pos_x];
            count++;
        }
    }

    return sum / count;
}

/**
 * Calculate the variance of a window in the image
 */
static double calculate_window_variance(unsigned char *data, int width, int window_x, int window_y, int window_size, double mean) {
    double sum = 0.0;
    int count = 0;

    for (int y = 0; y < window_size; y++) {
        for (int x = 0; x < window_size; x++) {
            int pos_x = window_x + x;
            int pos_y = window_y + y;
            double diff = data[pos_y * width + pos_x] - mean;
            sum += diff * diff;
            count++;
        }
    }

    return sum / count;
}

/**
 * Calculate the covariance of two windows
 */
static double calculate_window_covariance(unsigned char *data1, unsigned char *data2, int width1, int width2, 
                                         int window_x1, int window_y1, int window_x2, int window_y2, 
                                         int window_size, double mean1, double mean2) {
    double sum = 0.0;
    int count = 0;

    for (int y = 0; y < window_size; y++) {
        for (int x = 0; x < window_size; x++) {
            int pos_x1 = window_x1 + x;
            int pos_y1 = window_y1 + y;
            int pos_x2 = window_x2 + x;
            int pos_y2 = window_y2 + y;
            double diff1 = data1[pos_y1 * width1 + pos_x1] - mean1;
            double diff2 = data2[pos_y2 * width2 + pos_x2] - mean2;
            sum += diff1 * diff2;
            count++;
        }
    }

    return sum / count;
}

/**
 * Calculate the Structural Similarity Index (SSIM) between two images
 * Using a simplified version with small windows
 */
double calculate_ssim(PGMImage *img1, PGMImage *img2) {
    if (!img1 || !img2 || !img1->data || !img2->data) {
        fprintf(stderr, "Error: Invalid images for SSIM calculation\n");
        return -1.0;
    }

    // Check if dimensions match
    if (img1->width != img2->width || img1->height != img2->height) {
        fprintf(stderr, "Error: Image dimensions do not match for SSIM calculation\n");
        return -1.0;
    }

    // Constants for stability
    const double C1 = 6.5025; // (0.01 * 255)^2
    const double C2 = 58.5225; // (0.03 * 255)^2
    
    // Window size for SSIM calculation (typically 8x8 or 11x11)
    const int window_size = 8;
    
    // Number of windows to sample (for performance)
    const int max_windows = 100;
    
    double ssim_sum = 0.0;
    int window_count = 0;
    
    // Calculate SSIM for random windows
    srand(time(NULL));
    
    for (int i = 0; i < max_windows; i++) {
        // Random window position
        int window_x = rand() % (img1->width - window_size);
        int window_y = rand() % (img1->height - window_size);
        
        // Calculate statistics for the windows
        double mean1 = calculate_window_mean(img1->data, img1->width, window_x, window_y, window_size);
        double mean2 = calculate_window_mean(img2->data, img2->width, window_x, window_y, window_size);
        double var1 = calculate_window_variance(img1->data, img1->width, window_x, window_y, window_size, mean1);
        double var2 = calculate_window_variance(img2->data, img2->width, window_x, window_y, window_size, mean2);
        double covar = calculate_window_covariance(img1->data, img2->data, img1->width, img2->width, 
                                                window_x, window_y, window_x, window_y, 
                                                window_size, mean1, mean2);
        
        // Calculate SSIM for this window
        double numerator = (2 * mean1 * mean2 + C1) * (2 * covar + C2);
        double denominator = (mean1 * mean1 + mean2 * mean2 + C1) * (var1 + var2 + C2);
        double ssim = numerator / denominator;
        
        ssim_sum += ssim;
        window_count++;
    }
    
    return ssim_sum / window_count;
} 