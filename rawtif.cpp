// RAWK Copyright (C) 2014 Robert Kooima
//
// This program is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful, but WITH-
// OUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
// more details.

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <tiffio.h>
#include <sys/mman.h>

//------------------------------------------------------------------------------

void syserr()
{
    fprintf(stderr, "%s\n", strerror(errno));
    exit(-1);
}

uint8_t *rawopen(int *d, const char *name, size_t l)
{
    void *p;

    if ((*d = open(name, O_RDWR | O_TRUNC | O_CREAT, 0666)) == -1)
        syserr();

    if (ftruncate(*d, l) == -1)
        syserr();

    if ((p = mmap(0, l, PROT_READ | PROT_WRITE, MAP_SHARED, *d, 0)) == MAP_FAILED)
        syserr();

    return (uint8_t *) p;
}

void rawclose(int *d, uint8_t *p, size_t l)
{
    if (munmap(p, l) == -1)
        syserr();

    if (close(*d) == -1)
        syserr();
}

int main(int argc, char **argv)
{
    if (argc == 3)
    {
        if (TIFF *T = TIFFOpen(argv[1], "r"))
        {
            uint32 W = 0, H = 0;
            uint32 w = 0, h = 0;
            uint16 b = 0, c = 0;

            TIFFGetField(T, TIFFTAG_TILEWIDTH,       &W);
            TIFFGetField(T, TIFFTAG_TILELENGTH,      &H);
            TIFFGetField(T, TIFFTAG_IMAGEWIDTH,      &w);
            TIFFGetField(T, TIFFTAG_IMAGELENGTH,     &h);
            TIFFGetField(T, TIFFTAG_SAMPLESPERPIXEL, &c);
            TIFFGetField(T, TIFFTAG_BITSPERSAMPLE,   &b);

            size_t n = size_t(TIFFNumberOfTiles(T));
            size_t s = size_t(TIFFTileSize(T));
            size_t p = size_t(c * b / 8);
            size_t l = size_t(w * h * p);

            size_t cols = size_t((w + W - 1) / W);

            uint8_t src[s];

            int d = 0;

            if (uint8_t *dst = rawopen(&d, argv[2], l))
            {
                for (size_t t = 0; t < n; t++)
                {
                    size_t x = (t % cols) * size_t(W);
                    size_t y = (t / cols) * size_t(H);

                    if (TIFFReadTile(T, src, x, y, 0, 0) > 0)
                    {
                        for (size_t i = 0; i < H; i++)
                            memcpy(dst + p * ((y + i) * w + x),
                                   src + p * ((    i) * W), W * p);
                    }
                }
                rawclose(&d, dst, l);
            }
            TIFFClose(T);
        }
    }
    return 0;
}
