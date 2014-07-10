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

#ifndef IMAGE_THRESHOLD_HPP
#define IMAGE_THRESHOLD_HPP

//------------------------------------------------------------------------------

/// Threshold filter

class threshold : public image
{
public:
    /// Compare the values of each sample of image *L* with *value*. Give 1
    /// where *L* is greater than *value* and 0 elsewhere. Thresholding with
    /// a *value* of zero distinguishes samples of exactly zero, regardless of
    /// source data type.

    threshold(double value, image *L) : image(L), value(value) { }

    virtual pixel get(int i, int j) const
    {
        pixel p = L->get(i, j);

        return pixel(p.r > value ? 1 : 0,
                     p.g > value ? 1 : 0,
                     p.b > value ? 1 : 0,
                     p.a > value ? 1 : 0);
    }

    virtual void tweak(int a, int v)
    {
        if (a == 0) value += 0.001 * v;
    }

    virtual void doc(std::ostream& out) const
    {
        out << "threshold " << value;
    }

private:
    double value;
};

//------------------------------------------------------------------------------

#endif
