#include <stdio.h>
#include <windows.h>
#include <wingdi.h>

void SaveHBITMAPToFile(HBITMAP hBMP, const char *pszFile)
{
    BITMAP bmp;
    GetObject(hBMP, sizeof(BITMAP), &bmp);

    BITMAPFILEHEADER bmfHeader;
    bmfHeader.bfType = 0x4D42; // "BM"
    bmfHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bmp.bmWidth * bmp.bmHeight * (bmp.bmBitsPixel / 8);
    bmfHeader.bfReserved1 = 0;
    bmfHeader.bfReserved2 = 0;
    bmfHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    BITMAPINFOHEADER bi;
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmp.bmWidth;
    bi.biHeight = bmp.bmHeight * -1; // uncompressed RGB bitmaps if Height is negative, the bitmap is a top-down
    bi.biPlanes = 1;
    bi.biBitCount = bmp.bmBitsPixel;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    FILE *file = fopen(pszFile, "wb");
    if (file)
    {
        fwrite(&bmfHeader, sizeof(BITMAPFILEHEADER), 1, file);
        fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, file);
        int imageSize = bmp.bmWidth * bmp.bmHeight * (bmp.bmBitsPixel / 8);
        BYTE *imageData = (BYTE *)malloc(imageSize);
        if (GetBitmapBits(hBMP, imageSize, imageData))
        {
            fwrite(imageData, imageSize, 1, file);
        }
        free(imageData);
        fclose(file);
    }
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
