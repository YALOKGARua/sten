/**
 * glet_d3.c
 * Implementation of G-let D3 wavelet transform for steganography
 */

#include "../include/steganography.h"

/**
 * Apply Haar wavelet transform to a 1D array (in-place)
 * @param data Input/output data array
 * @param size Size of the array (must be a power of 2)
 */
static void haar_transform_1d(double *data, int size) {
    double *temp = (double *)malloc(size * sizeof(double));
    if (!temp) return;

    for (int stride = 1; stride < size; stride *= 2) {
        int half_stride = stride;
        stride *= 2;

        for (int i = 0; i < size; i += stride) {
            for (int j = 0; j < half_stride; j++) {
                int idx = i + j;
                int idx_pair = i + j + half_stride;
                
                temp[idx / 2] = (data[idx] + data[idx_pair]) / sqrt(2.0);
                temp[size / 2 + idx / 2] = (data[idx] - data[idx_pair]) / sqrt(2.0);
            }
        }

        for (int i = 0; i < size; i++) {
            data[i] = temp[i];
        }
    }

    free(temp);
}

/**
 * Apply inverse Haar wavelet transform to a 1D array (in-place)
 * @param data Input/output data array
 * @param size Size of the array (must be a power of 2)
 */
static void haar_inverse_1d(double *data, int size) {
    double *temp = (double *)malloc(size * sizeof(double));
    if (!temp) return;

    for (int stride = size; stride > 1; stride /= 2) {
        int half_stride = stride / 2;

        for (int i = 0; i < size; i += stride) {
            for (int j = 0; j < half_stride; j++) {
                int idx = i + j;
                int idx_pair = i + j + half_stride;
                
                // Using integer division for array indexing
                temp[idx] = (data[idx / 2] + data[size / 2 + idx / 2]) / sqrt(2.0);
                temp[idx_pair] = (data[idx / 2] - data[size / 2 + idx / 2]) / sqrt(2.0);
            }
        }

        for (int i = 0; i < size; i++) {
            data[i] = temp[i];
        }
    }

    free(temp);
}

/**
 * Apply G-let D3 forward transform to an image block
 * @param block Image block data (in-place transformation)
 * @param size Block size (must be a power of 2)
 */
void glet_d3_forward(double *block, int size) {
    double *temp_row = (double *)malloc(size * sizeof(double));
    if (!temp_row) return;

    // Apply 1D transform to each row
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            temp_row[j] = block[i * size + j];
        }
        haar_transform_1d(temp_row, size);
        for (int j = 0; j < size; j++) {
            block[i * size + j] = temp_row[j];
        }
    }

    // Apply 1D transform to each column
    for (int j = 0; j < size; j++) {
        for (int i = 0; i < size; i++) {
            temp_row[i] = block[i * size + j];
        }
        haar_transform_1d(temp_row, size);
        for (int i = 0; i < size; i++) {
            block[i * size + j] = temp_row[i];
        }
    }

    free(temp_row);
}

/**
 * Apply G-let D3 inverse transform to an image block
 * @param block Image block data (in-place transformation)
 * @param size Block size (must be a power of 2)
 */
void glet_d3_inverse(double *block, int size) {
    double *temp_row = (double *)malloc(size * sizeof(double));
    if (!temp_row) return;

    // Apply 1D inverse transform to each column
    for (int j = 0; j < size; j++) {
        for (int i = 0; i < size; i++) {
            temp_row[i] = block[i * size + j];
        }
        haar_inverse_1d(temp_row, size);
        for (int i = 0; i < size; i++) {
            block[i * size + j] = temp_row[i];
        }
    }

    // Apply 1D inverse transform to each row
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            temp_row[j] = block[i * size + j];
        }
        haar_inverse_1d(temp_row, size);
        for (int j = 0; j < size; j++) {
            block[i * size + j] = temp_row[j];
        }
    }

    free(temp_row);
} 