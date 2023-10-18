#include <stdio.h>
#include <windows.h>
#include <wingdi.h>

void SaveHBITMAPToFile(HBITMAP hBMP, const char* pszFile) {
    BITMAP bmp;
    // Retrieve information about the HBITMAP
    GetObject(hBMP, sizeof(BITMAP), &bmp);

    // Define the BMP file header
    BITMAPFILEHEADER bmfHeader;
    bmfHeader.bfType = 0x4D42; // "BM"
    bmfHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bmp.bmWidth * bmp.bmHeight * 3;
    bmfHeader.bfReserved1 = 0;
    bmfHeader.bfReserved2 = 0;
    bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // Create a device context compatible with the HBITMAP
    HDC hDC = GetDC(NULL);
    HDC hMemDC = CreateCompatibleDC(hDC);
    SelectObject(hMemDC, hBMP);

    // Define the BMP info header
    BITMAPINFOHEADER bi;
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmp.bmWidth;
    bi.biHeight = bmp.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 24; // Set to 24 bits per pixel
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    // Create the BMP file and open it for writing
    FILE* file = fopen(pszFile, "wb");
    if (file) {
        // Write the BMP file header
        fwrite(&bmfHeader, sizeof(BITMAPFILEHEADER), 1, file);
        // Write the BMP info header
        fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, file);
        // Calculate the size of the image data
        int imageSize = bmp.bmWidth * bmp.bmHeight * 3; // 3 bytes per pixel for 24-bit
        // Allocate memory for the image data
        BYTE* imageData = (BYTE*)malloc(imageSize);
        if (GetDIBits(hMemDC, hBMP, 0, bmp.bmHeight, imageData, (BITMAPINFO*)&bi, DIB_RGB_COLORS)) {
            // Write the image data to the file
            fwrite(imageData, imageSize, 1, file);
        }
        // Free the allocated memory
        free(imageData);
        // Close the BMP file
        fclose(file);
    }
    // Release the device contexts
    DeleteDC(hMemDC);
    ReleaseDC(NULL, hDC);
}

void getResolution(int *screenWidth, int *screenHeight)
{
    // screen size
    DEVMODE dm;
    dm.dmSize = sizeof(dm);
    if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm))
    {
        *screenWidth = dm.dmPelsWidth;
        *screenHeight = dm.dmPelsHeight;
    }
    else
    {
        printf("Failed to retrieve screen resolution.\n");
    }
}

int main()
{
    // Get the device context for the entire screen
    HDC screenDC = GetDC(NULL);

    // Create a compatible device context for the screen DC
    HDC memoryDC = CreateCompatibleDC(screenDC);

    int screenWidth;
    int screenHeight;
    getResolution(&screenWidth, &screenHeight);
    // Create a bitmap to store the screenshot
    HBITMAP hBitmap = CreateCompatibleBitmap(screenDC, screenWidth, screenHeight);

    // Select the bitmap into the memory DC
    SelectObject(memoryDC, hBitmap);

    // Copy the screen to the memory DC
    BitBlt(memoryDC, 0, 0, screenWidth, screenHeight, screenDC, 0, 0, SRCCOPY);

    // Save the screenshot to a file (you can change the file format)
    // In this example, it's saved as a BMP file.
    hBitmap = (HBITMAP)SelectObject(memoryDC, hBitmap);
    SaveHBITMAPToFile(hBitmap, "screenshot.bmp");

    // Clean up
    DeleteObject(hBitmap);
    DeleteDC(memoryDC);
    ReleaseDC(NULL, screenDC);

    return 0;
}
