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

#ifndef IMAGE_MATRIX_HPP
#define IMAGE_MATRIX_HPP

//------------------------------------------------------------------------------

/// Color matrix transformation filter

class matrix : public image
{
public:
    /// Apply a color space transformation to image *L*.

    matrix(std::vector<double> M, image *L) : image(L), M(M) { }

    virtual pixel get(int i, int j) const
    {
        pixel p = L->get(i, j);
        return pixel(p.r * M[ 0] + p.g * M[ 1] + p.b * M[ 2] + p.a * M[ 3],
                     p.r * M[ 4] + p.g * M[ 5] + p.b * M[ 6] + p.a * M[ 7],
                     p.r * M[ 8] + p.g * M[ 9] + p.b * M[10] + p.a * M[11],
                     p.r * M[12] + p.g * M[13] + p.b * M[14] + p.a * M[15]);
    }

    virtual void doc(std::ostream& out) const
    {
        out << "matrix";
    }

private:
    std::vector<double> M;
};

//------------------------------------------------------------------------------

/// RGB to YUV color space transformation

class rgb2yuv : public matrix
{
    /// Transform image *L* from the RGB to the YUV color space.

public:
    rgb2yuv(image *L) : matrix(std::vector<double>(v, v + 16), L) { }

private:
    static const double v[16];
};

const double rgb2yuv::v[16] = {
     0.29900,  0.58700,  0.11400, 0.00000,
    -0.14713,  0.28886,  0.43600, 0.00000,
     0.61500, -0.51499,  0.10001, 0.00000,
     0.00000,  0.00000,  0.00000, 1.00000,
};

//------------------------------------------------------------------------------

/// YUV to RGB color space transformation

class yuv2rgb : public matrix
{
public:
    /// Transform image *L* from the YUV to the RGB color space.

    yuv2rgb(image *L) : matrix(std::vector<double>(v, v + 16), L) { }

private:
    static const double v[16];
};

const double yuv2rgb::v[16] = {
     1.00000,  0.00000,  1.13983, 0.00000,
     1.00000, -0.39465, -0.58060, 0.00000,
     1.00000,  2.03211,  0.00000, 0.00000,
     0.00000,  0.00000,  0.00000, 1.00000,
};

//------------------------------------------------------------------------------

#endif
