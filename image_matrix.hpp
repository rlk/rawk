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
    /// Apply a color space transformation to image *L*. This transformation is
    /// given in the form of a matrix of size *rows* by *columns*. The input
    /// must have a depth equal to *columns* and the output will have a depth
    /// equal to *rows*. The vector *values* gives the matrix in row-major
    /// order.

    matrix(int rows, int columns, std::vector<double> values, image *L)
        : image(L), rows(rows), columns(columns), values(values)
    {
        if (columns != L->get_depth())
            throw std::runtime_error("Mismatched color matrix size");
    }

    virtual double get(int i, int j, int k) const
    {
        const int d = L->get_depth();
        double    v = 0;

        if (0 <= k && k < rows)
            for (int l = 0; l < d; l++)
                if (double w = values[k * columns + l])
                    v += w * L->get(i, j, l);
        return v;
    }

    virtual int get_depth() const
    {
        return rows;
    }

    virtual void doc(std::ostream& out) const
    {
        out << "matrix " << rows << " " << columns;
    }

private:
    int rows;
    int columns;
    std::vector<double> values;
};

//------------------------------------------------------------------------------

/// RGB to YUV color space transformation

class rgb2yuv : public matrix
{
    /// Transform image *L* from the RGB to the YUV color space.

public:
    rgb2yuv(image *L) : matrix(3, 3, std::vector<double>(v, v + 9), L) { }

private:
    static const double v[9];
};

const double rgb2yuv::v[9] = {
     0.29900,  0.58700,  0.11400,
    -0.14713,  0.28886,  0.43600,
     0.61500, -0.51499,  0.10001
};

//------------------------------------------------------------------------------

/// YUV to RGB color space transformation

class yuv2rgb : public matrix
{
public:
    /// Transform image *L* from the YUV to the RGB color space.

    yuv2rgb(image *L) : matrix(3, 3, std::vector<double>(v, v + 9), L) { }

private:
    static const double v[9];
};

const double yuv2rgb::v[9] = {
     1.00000,  0.00000,  1.13983,
     1.00000, -0.39465, -0.58060,
     1.00000,  2.03211,  0.00000
};

//------------------------------------------------------------------------------

#endif
