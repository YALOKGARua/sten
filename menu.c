#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void showMainMenu() {
    printf("==================================\n");
    printf("  G-let D3 PGM Steganography Tool\n");
    printf("==================================\n\n");
    printf("1. Embed Secret Image\n");
    printf("2. Extract Secret Image\n");
    printf("3. Assess Image Quality\n");
    printf("4. Exit\n");
}

void clearScreen() {
    system("cls");
}

void runCommand(const char* cmd) {
    printf("\nExecuting: %s\n\n", cmd);
    system(cmd);
}

void waitForEnter() {
    printf("\nPress Enter to continue...");
    getchar();
}

char* getInput(const char* prompt) {
    static char buffer[1024];
    printf("%s", prompt);
    fgets(buffer, sizeof(buffer), stdin);
    
    // Remove newline
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len-1] == '\n') {
        buffer[len-1] = '\0';
    }
    
    return buffer;
}

void embedMenu() {
    char command[2048] = {0};
    char* coverPath, *secretPath, *outputPath, *blockSize, *strength, *useRandom, *seed;
    
    clearScreen();
    printf("==== Embed Secret Image ====\n\n");
    
    coverPath = getInput("Enter cover image path (.pgm): ");
    secretPath = getInput("Enter secret image path (.pgm): ");
    outputPath = getInput("Enter output stego image path (.pgm): ");
    blockSize = getInput("Enter block size [8]: ");
    strength = getInput("Enter embedding strength (1-10) [5]: ");
    useRandom = getInput("Use random blocks? (y/n) [n]: ");
    
    // Build command
    sprintf(command, ".\\bin\\stego.exe embed \"%s\" \"%s\" \"%s\"", 
            coverPath, secretPath, outputPath);
    
    if (strlen(blockSize) > 0) {
        strcat(command, " -b ");
        strcat(command, blockSize);
    }
    
    if (strlen(strength) > 0) {
        strcat(command, " -s ");
        strcat(command, strength);
    }
    
    if (useRandom[0] == 'y' || useRandom[0] == 'Y') {
        strcat(command, " -r");
        
        seed = getInput("Enter random seed [12345]: ");
        if (strlen(seed) > 0) {
            strcat(command, " -seed ");
            strcat(command, seed);
        }
    }
    
    runCommand(command);
    waitForEnter();
}

void extractMenu() {
    char command[2048] = {0};
    char* stegoPath, *outputPath, *width, *height, *blockSize, *strength, *useRandom, *seed;
    char dimensions[100] = {0};
    
    clearScreen();
    printf("==== Extract Secret Image ====\n\n");
    
    stegoPath = getInput("Enter stego image path (.pgm): ");
    outputPath = getInput("Enter output image path (.pgm): ");
    
    printf("Enter secret image dimensions (leave blank if unknown):\n");
    width = getInput("Width: ");
    height = getInput("Height: ");
    
    blockSize = getInput("Enter block size [8]: ");
    strength = getInput("Enter embedding strength (1-10) [5]: ");
    useRandom = getInput("Was random blocks used? (y/n) [n]: ");
    
    // Build command
    sprintf(command, ".\\bin\\stego.exe extract \"%s\" \"%s\"", 
            stegoPath, outputPath);
    
    if (strlen(width) > 0 && strlen(height) > 0) {
        sprintf(dimensions, " %s %s", width, height);
        strcat(command, dimensions);
    }
    
    if (strlen(blockSize) > 0) {
        strcat(command, " -b ");
        strcat(command, blockSize);
    }
    
    if (strlen(strength) > 0) {
        strcat(command, " -s ");
        strcat(command, strength);
    }
    
    if (useRandom[0] == 'y' || useRandom[0] == 'Y') {
        strcat(command, " -r");
        
        seed = getInput("Enter the same random seed: ");
        if (strlen(seed) > 0) {
            strcat(command, " -seed ");
            strcat(command, seed);
        }
    }
    
    runCommand(command);
    waitForEnter();
}

void assessMenu() {
    char command[2048] = {0};
    char* originalPath, *modifiedPath;
    
    clearScreen();
    printf("==== Assess Image Quality ====\n\n");
    
    originalPath = getInput("Enter original image path (.pgm): ");
    modifiedPath = getInput("Enter modified/stego image path (.pgm): ");
    
    // Build command
    sprintf(command, ".\\bin\\stego.exe assess \"%s\" \"%s\"", 
            originalPath, modifiedPath);
    
    runCommand(command);
    waitForEnter();
}

int main() {
    int choice = 0;
    char input[10];
    
    while (1) {
        clearScreen();
        showMainMenu();
        
        printf("\nEnter choice (1-4): ");
        fgets(input, sizeof(input), stdin);
        choice = atoi(input);
        
        switch (choice) {
            case 1:
                embedMenu();
                break;
            case 2:
                extractMenu();
                break;
            case 3:
                assessMenu();
                break;
            case 4:
                clearScreen();
                printf("Thank you for using the G-let D3 PGM Steganography Tool!\n");
                return 0;
            default:
                printf("Invalid choice. Press Enter to try again...");
                getchar();
                break;
        }
    }
    
    return 0;
} 