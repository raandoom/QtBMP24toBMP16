#include <QDataStream>
#include <QFile>
#include "bmp.h"
#include <QDebug>

#define ORIENT_DEFAULT      0
#define ORIENT_FROM_TOP     1
#define ORIENT_FROM_BOTTOM  2

void initStruct(char* data, unsigned int count)
{
    while (count !=0)
    {
        count--;
        data[count] = 0;
    }
}
//==================================================

unsigned short checkBMPbpp(QString pathImage)
{
    // check for "BM" and return bpp
    QFile readf(pathImage);
    readf.open(QIODevice::ReadOnly);
    QDataStream streamIn(&readf);
    streamIn.setByteOrder(QDataStream::LittleEndian);
    unsigned short imageType;
    streamIn >> imageType;
    if (imageType != 0x4D42)
    {
        readf.close();
        return 0;
    }
    readf.seek(28);
    unsigned short bpp;
    streamIn >> bpp;
    readf.close();
    return bpp;
}
//==================================================
void convert24to16(QString srcPath, QString destPath, int orient)
{
    bool turn;

    QFile readf(srcPath);
    QFile writef(destPath);
    readf.open(QIODevice::ReadOnly);
    writef.open(QIODevice::WriteOnly);
    QDataStream streamIn(&readf);
    streamIn.setByteOrder(QDataStream::LittleEndian);
    QDataStream streamOut(&writef);
    streamOut.setByteOrder(QDataStream::LittleEndian);

    BITMAPFILEHEADER image24;
    initStruct((char*)&image24, 14);
    // make bmp header for 24
    streamIn >> image24.bfType;
    streamIn >> image24.bfSize;
    streamIn >> image24.bfReserved1;
    streamIn >> image24.bfReserved2;
    streamIn >> image24.bfOffBits;

    BITMAPINFOHEADER info24;
    initStruct((char*)&info24, 40);
    // make bmp info for 24
    streamIn >> info24.biSize;
    streamIn >> info24.biWidth;
    streamIn >> info24.biHeight;
    streamIn >> info24.biPlanes;
    streamIn >> info24.biBitCount;
    streamIn >> info24.biCompression;
    streamIn >> info24.biSizeImage;
    streamIn >> info24.biXPelsPerMeter;
    streamIn >> info24.biYPelsPerMeter;
    streamIn >> info24.biClrUsed;
    streamIn >> info24.biClrImportant;

    // check zero-padding
    int srcPadding = info24.biWidth % 4;
    int destPadding = (info24.biWidth * 2) % 4;

    BITMAPFILEHEADER image16;
    initStruct((char*)&image16, 14);
    // make bmp header for 16
    image16.bfOffBits = 0x42;
    image16.bfReserved2 = image24.bfReserved2;
    image16.bfReserved1 = image24.bfReserved1;
    image16.bfSize =
            ((info24.biWidth * 2) + destPadding) * qAbs(info24.biHeight) +
            image16.bfOffBits;
    image16.bfType = image24.bfType;

    BITMAPINFOHEADER info16;
    initStruct((char*)&info16, 40);
    // make bmp info for 16
    info16.biSize = 0x28;
    info16.biWidth = info24.biWidth;

    if ((orient == ORIENT_DEFAULT) ||
            ((orient == ORIENT_FROM_TOP) && (info24.biHeight < 0)) ||
            ((orient == ORIENT_FROM_BOTTOM) && (info24.biHeight > 0)))
    {
        turn = false;
        info16.biHeight = info24.biHeight;
    }
    else
    {
        turn = true;
        info16.biHeight = - info24.biHeight;
    }

    info16.biPlanes = 1;
    info16.biBitCount = 0x10;
    info16.biCompression = 3;
    info16.biSizeImage = image16.bfSize - image16.bfOffBits;
    info16.biXPelsPerMeter = info24.biXPelsPerMeter;
    info16.biYPelsPerMeter = info24.biYPelsPerMeter;
    info16.biClrUsed = 0;
    info16.biClrImportant = 0;

    streamOut << image16.bfType;
    streamOut << image16.bfSize;
    streamOut << image16.bfReserved1;
    streamOut << image16.bfReserved2;
    streamOut << image16.bfOffBits;

    streamOut << info16.biSize;
    streamOut << info16.biWidth;
    streamOut << info16.biHeight;
    streamOut << info16.biPlanes;
    streamOut << info16.biBitCount;
    streamOut << info16.biCompression;
    streamOut << info16.biSizeImage;
    streamOut << info16.biXPelsPerMeter;
    streamOut << info16.biYPelsPerMeter;
    streamOut << info16.biClrUsed;
    streamOut << info16.biClrImportant;
    streamOut << 0xF800; // red
    streamOut << 0x7E0; // green
    streamOut << 0x1F; // blue

    // start convert
    int h = qAbs(info24.biHeight);
    unsigned char r, g, b;
    unsigned short rgb16;

    readf.seek(image24.bfOffBits);
    if (turn)
        writef.seek(image16.bfOffBits +
                    ((info16.biWidth * 2) + destPadding) * (h - 1));
    else
        writef.seek(image16.bfOffBits);

    while (h != 0)
    {
        unsigned int w = info24.biWidth;
        while (w != 0)
        {
            streamIn >> b;
            streamIn >> g;
            streamIn >> r;
            r = r >> 3;
            g = g >> 2;
            b = b >> 3;
            rgb16 = (r << 11) + (g << 5) + b;
            streamOut << rgb16;
            w--;
        }
        if (srcPadding != 0)
        {
            unsigned char trash;
            for (int i = 0; i < srcPadding; i++) // read unneeded bytes to trash
            {
                streamIn >> trash;
            }
        }
        if (destPadding != 0)
        {
            unsigned char p = 0;
            for (int i = 0; i < destPadding; i++)
            {
                streamOut << p;
            }
        }
        h--;
        if (turn)
            writef.seek(image16.bfOffBits +
                        ((info16.biWidth * 2) + destPadding) * (h - 1));
    }
    readf.close();
    writef.close();
}
