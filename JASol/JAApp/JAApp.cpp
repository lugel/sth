#include "pch.h"
#include <iostream>
#include <windows.h>
#include <io.h>
#include <thread>
#include <cstdlib>
#include "JACDLL.h"

extern "C" int __stdcall AsmVal(); // function from assembler dll
void findBorders(BITMAPINFOHEADER *bih, int noOfBytes, BYTE *bytesDIB); // function from C dll

// bitmap structure
typedef struct Bitmap
{
	BITMAPFILEHEADER bfh;
	BITMAPINFOHEADER *bih;
	long int bmpsize;
	void* bmp;
	byte* bytesDIB;
	void* colorPalette;
	int noOfBytes;
}Bitmap;

// checking the size of the bitmap
int howManyBytes(BITMAPINFOHEADER *bhead) // return the number of bytes in bitmap
{
	int bytesQuant;
	bytesQuant = ((int)bhead->biWidth * bhead->biBitCount - 1) / 8 + 1;
	while (bytesQuant % 4)
		bytesQuant++;
	return bytesQuant;
}

// loadnig the bitmap
int loadBitmap(const char *fname, Bitmap *myBmp)
{
	FILE *myFile;
	int everythingIsOk;
	long int size;

	myFile = fopen(fname, "rb");

	if (myFile == NULL)
		return FALSE;

	size = _filelength(_fileno(myFile)) - sizeof(BITMAPFILEHEADER);

	if (!(fread(&(myBmp->bfh), sizeof(BITMAPFILEHEADER), 1, myFile)))
	{
		fclose(myFile);
		return FALSE;
	}

	myBmp->bmp = malloc(size);
	if (myBmp->bmp == NULL)
	{
		fclose(myFile);
		return FALSE;
	}

	everythingIsOk = fread(myBmp->bmp, size, 1, myFile);

	fclose(myFile);

	if (!everythingIsOk)
	{
		free(myBmp->bmp);
		myBmp->bmp = NULL;
		return FALSE;
	}


	myBmp->bih = (BITMAPINFOHEADER *)(myBmp->bmp);

	myBmp->bmpsize = size;
	myBmp->bytesDIB = (byte *)(myBmp->bmp) + (myBmp->bih)->biSize;
	myBmp->colorPalette = (void *)((byte *)(myBmp->bmp) + (myBmp->bih)->biSize);

	myBmp->noOfBytes = howManyBytes(myBmp->bih);

	return TRUE;
}

// saving the bitmap
int saveBitmap(const char *fname, Bitmap *myBmp)
{
	FILE *f;
	int saveOk;

	f = fopen(fname, "wb");

	if (f == NULL)
		return FALSE;

	fwrite((void *)&(myBmp->bfh), sizeof(BITMAPFILEHEADER), 1, f);

	if (myBmp->bmpsize > 0)
		saveOk = (fwrite(myBmp->bmp, myBmp->bmpsize, 1, f) == 1);
	else
		saveOk = TRUE;

	fclose(f);

	return saveOk;
}

int main(int argc, char *argv[])
{
	int nOfThreads = std::thread::hardware_concurrency(); // optimal number of threads
	Bitmap bmp;

	if (argc < 2)
		printf("You didn't specified how many threads you want to use so optimal number will be used: %i\n", nOfThreads);
	else
	{
		nOfThreads = atoi(argv[1]);
		printf("Using the specified number of threads: %i\n", nOfThreads);
	}

	std::thread **thTable;
	thTable = new std::thread*[nOfThreads];

	for (size_t i = 0; i < nOfThreads; i++)
	{
		thTable[i] = new std::thread();
	}

	loadBitmap("bitmap.bmp", &bmp);
	findBorders(bmp.bih, bmp.noOfBytes, bmp.bytesDIB);
	saveBitmap("result.bmp", &bmp);
	printf("Asm: %i", AsmVal());

	for (size_t i = 0; i < nOfThreads; i++)
	{
		delete thTable[i];
	}
	return 0;
}

//dostepne rejestry: AVX (AVX2)
//rozkazy:
// www.felixcloutier.com/x86/pcmpeqb:pcmpeqw:pcmpeqd
//VPCMPEQB - Compare packed bytes in xmm3 / m128 and xmm2 for equality. AVX
//VPCMPEQB - Compare packed bytes in ymm3/m256 and ymm2 for equality. AVX2

