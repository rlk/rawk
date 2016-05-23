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

uint8_t *rawread(int *d, const char *name, size_t l)
{
    void *p;

    if ((*d = open(name, O_RDONLY, 0666)) == -1)
        syserr();

    if ((p = mmap(0, l, PROT_READ, MAP_SHARED, *d, 0)) == MAP_FAILED)
        syserr();

    return (uint8_t *) p;
}

uint8_t *rawwrite(int *d, const char *name, size_t l)
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

int extcmp(const char *name, const char *ext)
{
    return strcasecmp(name + strlen(name) - strlen(ext), ext);
}

void tif2raw(const char *in, const char *out)
{
    if (TIFF *T = TIFFOpen(in, "r"))
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

        size_t p = size_t(c * b / 8);
        size_t l = size_t(w * h * p);

        int d = 0;

        if (uint8_t *dst = rawwrite(&d, out, l))
        {
            if (W && H)
            {
                size_t n = size_t(TIFFNumberOfTiles(T));
                size_t s = size_t(TIFFTileSize(T));
                size_t C = size_t((w + W - 1) / W);

                uint8_t src[s];

                for (size_t t = 0; t < n; t++)
                {
                    size_t x = (t % C) * size_t(W);
                    size_t y = (t / C) * size_t(H);

                    if (TIFFReadTile(T, src, x, y, 0, 0) > 0)
                    {
                        for (size_t i = 0; i < H; i++)
                            memcpy(dst + p * ((y + i) * w + x),
                                   src + p * ((    i) * W), W * p);
                    }
                }
            }
            else
            {
                for (size_t i = 0; i < h; i++)
                    TIFFReadScanline(T, dst + i * w * p, i, 0);
            }
            rawclose(&d, dst, l);
        }
        TIFFClose(T);
    }
}

void raw2tif(const char *in, const char *out, uint32 h, uint32 w, uint16 c, uint16 b, uint16 f)
{
    size_t p = size_t(c * b / 8);
    size_t l = size_t(w * h * p);

    int d = 0;

    if (uint8_t *src = rawread(&d, in, l))
    {
        if (TIFF *T = TIFFOpen(out, "w8"))
        {
            TIFFSetField(T, TIFFTAG_IMAGELENGTH,     h);
            TIFFSetField(T, TIFFTAG_IMAGEWIDTH,      w);
            TIFFSetField(T, TIFFTAG_SAMPLESPERPIXEL, c);
            TIFFSetField(T, TIFFTAG_BITSPERSAMPLE,   b);
            TIFFSetField(T, TIFFTAG_SAMPLEFORMAT,    f);
            TIFFSetField(T, TIFFTAG_PLANARCONFIG,    PLANARCONFIG_CONTIG);

            if (c == 3)
                TIFFSetField(T, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
            else
                TIFFSetField(T, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);

            for (uint32 i = 0; i < h; i++)
                TIFFWriteScanline(T, src + p * w * i, i, 0);

            TIFFClose(T);
        }
        rawclose(&d, src, l);
    }
}

int main(int argc, char **argv)
{
    if      (argc == 3 && extcmp(argv[1], ".tif") == 0)
        tif2raw(argv[1], argv[2]);

    else if (argc == 8 && extcmp(argv[2], ".tif") == 0)
        raw2tif(argv[1], argv[2], int(strtol(argv[3], 0, 0)),
                                  int(strtol(argv[4], 0, 0)),
                                  int(strtol(argv[5], 0, 0)),
                                  int(strtol(argv[6], 0, 0)),
                                  int(strtol(argv[7], 0, 0)));

    else
        fprintf(stderr, "Usage:\t%s input.tif output.raw\n"
                              "\t%s input.raw output.tif "
                              "height width channels bitdepth format\n"
                              "\t\tformat 1: unsigned integer\n"
                              "\t\tformat 2: signed integer\n"
                              "\t\tformat 3: floating point\n",
                              argv[0], argv[0]);
    return 0;
}
