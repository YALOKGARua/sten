/**
 * stego_gui.c
 * Simple graphical user interface for the steganography program
 */

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>

// GUI Control IDs
#define ID_COVER_BUTTON 101
#define ID_SECRET_BUTTON 102
#define ID_OUTPUT_BUTTON 103
#define ID_EMBED_BUTTON 104
#define ID_EXTRACT_BUTTON 105
#define ID_ASSESS_BUTTON 106
#define ID_BLOCK_SIZE 107
#define ID_STRENGTH 108
#define ID_RANDOM_CHECK 109
#define ID_SEED 110
#define ID_STATUS 111
#define ID_SECRET_WIDTH 112
#define ID_SECRET_HEIGHT 113

// Global variables
HWND hCoverEdit, hSecretEdit, hOutputEdit;
HWND hBlockSizeEdit, hStrengthEdit, hSeedEdit;
HWND hRandomCheck, hStatusText;
HWND hSecretWidthEdit, hSecretHeightEdit;
HINSTANCE hInst;
char szClassName[] = "StegoGUI";

// Function prototypes
LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
void CreateControls(HWND);
void ExecuteStego(HWND, int);
void BrowseFile(HWND, int);

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, 
                   LPSTR lpszArgument, int nCmdShow) {
    HWND hwnd;
    MSG messages;
    WNDCLASSEX wincl;

    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof(WNDCLASSEX);
    wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);

    if (!RegisterClassEx(&wincl))
        return 0;

    hInst = hThisInstance;

    hwnd = CreateWindowEx(
        0,
        szClassName,
        "G-let D3 PGM Steganography",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        640,
        480,
        HWND_DESKTOP,
        NULL,
        hThisInstance,
        NULL
    );

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&messages, NULL, 0, 0)) {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

    return messages.wParam;
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE:
            CreateControls(hwnd);
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_COVER_BUTTON:
                    BrowseFile(hwnd, ID_COVER_BUTTON);
                    break;
                case ID_SECRET_BUTTON:
                    BrowseFile(hwnd, ID_SECRET_BUTTON);
                    break;
                case ID_OUTPUT_BUTTON:
                    BrowseFile(hwnd, ID_OUTPUT_BUTTON);
                    break;
                case ID_EMBED_BUTTON:
                    ExecuteStego(hwnd, 1); // 1 for embed
                    break;
                case ID_EXTRACT_BUTTON:
                    ExecuteStego(hwnd, 2); // 2 for extract
                    break;
                case ID_ASSESS_BUTTON:
                    ExecuteStego(hwnd, 3); // 3 for assess
                    break;
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

void CreateControls(HWND hwnd) {
    // Cover image section
    CreateWindow("STATIC", "Cover/Stego Image:", WS_VISIBLE | WS_CHILD,
                 20, 20, 120, 20, hwnd, NULL, hInst, NULL);
    
    hCoverEdit = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                            20, 45, 400, 25, hwnd, NULL, hInst, NULL);
    
    CreateWindow("BUTTON", "Browse...", WS_VISIBLE | WS_CHILD,
                 430, 45, 80, 25, hwnd, (HMENU)ID_COVER_BUTTON, hInst, NULL);

    // Secret image section
    CreateWindow("STATIC", "Secret Image:", WS_VISIBLE | WS_CHILD,
                 20, 80, 120, 20, hwnd, NULL, hInst, NULL);
    
    hSecretEdit = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                             20, 105, 400, 25, hwnd, NULL, hInst, NULL);
    
    CreateWindow("BUTTON", "Browse...", WS_VISIBLE | WS_CHILD,
                 430, 105, 80, 25, hwnd, (HMENU)ID_SECRET_BUTTON, hInst, NULL);

    // Output image section
    CreateWindow("STATIC", "Output Image:", WS_VISIBLE | WS_CHILD,
                 20, 140, 120, 20, hwnd, NULL, hInst, NULL);
    
    hOutputEdit = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
                             20, 165, 400, 25, hwnd, NULL, hInst, NULL);
    
    CreateWindow("BUTTON", "Browse...", WS_VISIBLE | WS_CHILD,
                 430, 165, 80, 25, hwnd, (HMENU)ID_OUTPUT_BUTTON, hInst, NULL);

    // Secret image dimensions (for extraction)
    CreateWindow("STATIC", "Secret Image Dimensions (for extraction):", WS_VISIBLE | WS_CHILD,
                 20, 200, 280, 20, hwnd, NULL, hInst, NULL);
    
    CreateWindow("STATIC", "Width:", WS_VISIBLE | WS_CHILD,
                 20, 225, 50, 20, hwnd, NULL, hInst, NULL);
    
    hSecretWidthEdit = CreateWindow("EDIT", "256", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
                                  75, 225, 60, 25, hwnd, (HMENU)ID_SECRET_WIDTH, hInst, NULL);
    
    CreateWindow("STATIC", "Height:", WS_VISIBLE | WS_CHILD,
                 150, 225, 50, 20, hwnd, NULL, hInst, NULL);
    
    hSecretHeightEdit = CreateWindow("EDIT", "256", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
                                   205, 225, 60, 25, hwnd, (HMENU)ID_SECRET_HEIGHT, hInst, NULL);

    // Configuration section
    CreateWindow("STATIC", "Configuration:", WS_VISIBLE | WS_CHILD,
                 20, 260, 120, 20, hwnd, NULL, hInst, NULL);
    
    CreateWindow("STATIC", "Block Size:", WS_VISIBLE | WS_CHILD,
                 20, 285, 70, 20, hwnd, NULL, hInst, NULL);
    
    hBlockSizeEdit = CreateWindow("EDIT", "8", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
                                95, 285, 40, 25, hwnd, (HMENU)ID_BLOCK_SIZE, hInst, NULL);
    
    CreateWindow("STATIC", "Strength (1-10):", WS_VISIBLE | WS_CHILD,
                 150, 285, 100, 20, hwnd, NULL, hInst, NULL);
    
    hStrengthEdit = CreateWindow("EDIT", "5", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
                               255, 285, 40, 25, hwnd, (HMENU)ID_STRENGTH, hInst, NULL);
    
    hRandomCheck = CreateWindow("BUTTON", "Use Random Blocks", WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
                              20, 320, 150, 20, hwnd, (HMENU)ID_RANDOM_CHECK, hInst, NULL);
    
    CreateWindow("STATIC", "Random Seed:", WS_VISIBLE | WS_CHILD,
                 180, 320, 80, 20, hwnd, NULL, hInst, NULL);
    
    hSeedEdit = CreateWindow("EDIT", "12345", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
                           265, 320, 80, 25, hwnd, (HMENU)ID_SEED, hInst, NULL);

    // Action buttons
    CreateWindow("BUTTON", "Embed Image", WS_VISIBLE | WS_CHILD,
                 20, 360, 120, 30, hwnd, (HMENU)ID_EMBED_BUTTON, hInst, NULL);
    
    CreateWindow("BUTTON", "Extract Image", WS_VISIBLE | WS_CHILD,
                 150, 360, 120, 30, hwnd, (HMENU)ID_EXTRACT_BUTTON, hInst, NULL);
    
    CreateWindow("BUTTON", "Assess Quality", WS_VISIBLE | WS_CHILD,
                 280, 360, 120, 30, hwnd, (HMENU)ID_ASSESS_BUTTON, hInst, NULL);

    // Status text
    hStatusText = CreateWindow("STATIC", "Ready", WS_VISIBLE | WS_CHILD | SS_CENTER,
                             20, 400, 580, 20, hwnd, (HMENU)ID_STATUS, hInst, NULL);
}

void BrowseFile(HWND hwnd, int buttonId) {
    OPENFILENAME ofn;
    char szFile[260] = {0};
    HWND hEdit;

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "PGM Files\0*.pgm\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    // Determine which edit control to update
    switch (buttonId) {
        case ID_COVER_BUTTON:
            hEdit = hCoverEdit;
            ofn.lpstrTitle = "Select Cover/Stego Image";
            break;
        case ID_SECRET_BUTTON:
            hEdit = hSecretEdit;
            ofn.lpstrTitle = "Select Secret Image";
            break;
        case ID_OUTPUT_BUTTON:
            hEdit = hOutputEdit;
            ofn.lpstrTitle = "Select Output Image";
            ofn.Flags = OFN_PATHMUSTEXIST;
            break;
        default:
            return;
    }

    if (GetOpenFileName(&ofn)) {
        SetWindowText(hEdit, szFile);
    }
}

void ExecuteStego(HWND hwnd, int mode) {
    char coverPath[260], secretPath[260], outputPath[260];
    char blockSize[10], strength[10], seed[20];
    char command[1024];
    int useRandom;
    char secretWidth[10], secretHeight[10];
    
    // Get text from edit controls
    GetWindowText(hCoverEdit, coverPath, 260);
    GetWindowText(hSecretEdit, secretPath, 260);
    GetWindowText(hOutputEdit, outputPath, 260);
    GetWindowText(hBlockSizeEdit, blockSize, 10);
    GetWindowText(hStrengthEdit, strength, 10);
    GetWindowText(hSeedEdit, seed, 20);
    GetWindowText(hSecretWidthEdit, secretWidth, 10);
    GetWindowText(hSecretHeightEdit, secretHeight, 10);
    
    // Check random blocks checkbox
    useRandom = SendMessage(hRandomCheck, BM_GETCHECK, 0, 0);

    // Validate inputs according to mode
    if (mode == 1) { // Embed
        if (strlen(coverPath) == 0 || strlen(secretPath) == 0 || strlen(outputPath) == 0) {
            MessageBox(hwnd, "Please provide all required file paths.", "Input Error", MB_ICONERROR);
            return;
        }
    } else if (mode == 2) { // Extract
        if (strlen(coverPath) == 0 || strlen(outputPath) == 0) {
            MessageBox(hwnd, "Please provide stego image and output paths.", "Input Error", MB_ICONERROR);
            return;
        }
    } else if (mode == 3) { // Assess
        if (strlen(coverPath) == 0 || strlen(secretPath) == 0) {
            MessageBox(hwnd, "Please provide original and modified image paths.", "Input Error", MB_ICONERROR);
            return;
        }
    }

    // Construct the command
    strcpy(command, ".\\bin\\stego.exe ");
    
    if (mode == 1) { // Embed
        strcat(command, "embed \"");
        strcat(command, coverPath);
        strcat(command, "\" \"");
        strcat(command, secretPath);
        strcat(command, "\" \"");
        strcat(command, outputPath);
        strcat(command, "\"");
        
        // Add options
        strcat(command, " -b ");
        strcat(command, blockSize);
        strcat(command, " -s ");
        strcat(command, strength);
        
        if (useRandom) {
            strcat(command, " -r -seed ");
            strcat(command, seed);
        }
    } else if (mode == 2) { // Extract
        strcat(command, "extract \"");
        strcat(command, coverPath);
        strcat(command, "\" \"");
        strcat(command, outputPath);
        strcat(command, "\"");
        
        // Add dimensions if provided
        if (strlen(secretWidth) > 0 && strlen(secretHeight) > 0) {
            strcat(command, " ");
            strcat(command, secretWidth);
            strcat(command, " ");
            strcat(command, secretHeight);
        }
        
        // Add options
        strcat(command, " -b ");
        strcat(command, blockSize);
        strcat(command, " -s ");
        strcat(command, strength);
        
        if (useRandom) {
            strcat(command, " -r -seed ");
            strcat(command, seed);
        }
    } else if (mode == 3) { // Assess
        strcat(command, "assess \"");
        strcat(command, coverPath);
        strcat(command, "\" \"");
        strcat(command, secretPath);
        strcat(command, "\"");
    }

    // Update status
    SetWindowText(hStatusText, "Processing...");

    // Execute the command
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    
    if (CreateProcess(NULL, command, NULL, NULL, FALSE, 
                      CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        // Wait for the process to finish
        WaitForSingleObject(pi.hProcess, INFINITE);
        
        // Close process and thread handles
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        
        SetWindowText(hStatusText, "Operation completed successfully!");
        
        // Show the result
        if (mode == 1 || mode == 2) {
            char message[512];
            sprintf(message, "Operation completed!\n\nOutput file: %s", outputPath);
            MessageBox(hwnd, message, "Success", MB_ICONINFORMATION);
        }
    } else {
        SetWindowText(hStatusText, "Error executing command");
        MessageBox(hwnd, "Failed to execute the steganography process.", "Error", MB_ICONERROR);
    }
} 