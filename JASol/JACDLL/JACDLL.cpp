// JACDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "JACDLL.h"

long int BmpPixel(int x, int y, unsigned long color, int change, BITMAPINFOHEADER *bih, int noOfBytes, BYTE *bytesDIB)
{
	BYTE *line, *chosenPixel;

	line = bytesDIB + (bih->biHeight - y - 1)*noOfBytes; // setting the correct line

	chosenPixel = line + 3 * x; // correct position in the line

	// changing the RGB values of the chosen pixel
	if (change)
	{
		chosenPixel[0] = GetRValue(color);
		chosenPixel[1] = GetGValue(color);
		chosenPixel[2] = GetBValue(color);
	}

	color = RGB(chosenPixel[0], chosenPixel[1], chosenPixel[2]);

	return color;
}

void findBorders(BITMAPINFOHEADER *bih, int noOfBytes, BYTE *bytesDIB)
{
	int i, j;
	int* table = (int*)malloc(bih->biWidth * (bih->biHeight * sizeof(int))); // 2d array with the same size as bmp

	// check the whole bmp pixel by pixel and if there is a different color horizontally or vertically put 1 in array,
	// otherwise put 0 
	for (i = 1; i < bih->biWidth - 1; i++)
	{
		for (j = 1; j < bih->biHeight - 1; j++)
		{
			// checking
			if ((BmpPixel(i, j, 0, 0, bih, noOfBytes, bytesDIB) != (BmpPixel(i, j - 1, 0, 0, bih, noOfBytes, bytesDIB)))
				|| (BmpPixel(i, j, 0, 0, bih, noOfBytes, bytesDIB) != (BmpPixel(i, j + 1, 0, 0, bih, noOfBytes, bytesDIB)))
				|| (BmpPixel(i, j, 0, 0, bih, noOfBytes, bytesDIB) != (BmpPixel(i - 1, j, 0, 0, bih, noOfBytes, bytesDIB)))
				|| (BmpPixel(i, j, 0, 0, bih, noOfBytes, bytesDIB) != (BmpPixel(i + 1, j, 0, 0, bih, noOfBytes, bytesDIB))))
				*(table + i * bih->biHeight + j) = 1; // different color
			else
				*(table + i * bih->biHeight + j) = 0; // same color
		}
	}

	// coloring the bmp according to numbers in the 2d array
	for (i = 0; i < bih->biWidth; i++)
	{
		for (j = 0; j < bih->biHeight; j++)
		{
			// checking the number
			if (*(table + i * bih->biHeight + j))
				BmpPixel(i, j, RGB(0, 0, 0), 1, bih, noOfBytes, bytesDIB); // 1 = true, black color
			else
				BmpPixel(i, j, RGB(255, 255, 255), 1, bih, noOfBytes, bytesDIB); // 0 = false, white color
		}
	}
}
