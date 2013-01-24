#ifndef BMP_H
#define BMP_H

struct BITMAPFILEHEADER
{
  unsigned short    bfType; // "BM" = 0x4d42
  unsigned int      bfSize; // file size
  unsigned short    bfReserved1; // reserved = 0
  unsigned short    bfReserved2; // reserved = 0
  unsigned int      bfOffBits; // image offset
};

struct BITMAPINFOHEADER
{
  unsigned int      biSize; // size of struct
  unsigned int      biWidth; // width
  signed int        biHeight; // height
  unsigned short    biPlanes; // in bmp = 1
  unsigned short    biBitCount; // bpp
  unsigned int      biCompression; // 3 with pallete, 0 without pallete
  unsigned int      biSizeImage; // raw size
  unsigned int      biXPelsPerMeter; // px per m (0xec4)
  unsigned int      biYPelsPerMeter; // px per m (0xec4)
  unsigned int      biClrUsed; // 0
  unsigned int      biClrImportant; // 0
};

#endif // BMP_H
