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

#ifndef IMAGE_ARITHMETIC_HPP
#define IMAGE_ARITHMETIC_HPP

//------------------------------------------------------------------------------

/// Sum operator

class sum : public image
{
public:
    /// Add all samples of image *L* and image *R*. The width, height, and depth
    /// of the result are the larger of the widths, heights, and depths of the
    /// inputs.

    sum(image *L, image *R) : image(L, R) { }

    virtual pixel get(int i, int j) const
    {
        pixel l = L->get(i, j);
        pixel r = R->get(i, j);
        return pixel(l.r + r.r, l.g + r.g, l.b + r.b, l.a + r.a);
    }

    virtual void doc(std::ostream& out) const
    {
        out << "sum";
    }
};

//------------------------------------------------------------------------------

/// Difference operator

class difference : public image
{
public:
    /// Subtract all samples of image *R* from image *L*. The width, height, and
    /// depth of the result are the larger of the widths, heights, and depths of
    /// the inputs.

    difference(image *L, image *R) : image(L, R) { }

    virtual pixel get(int i, int j) const
    {
        pixel l = L->get(i, j);
        pixel r = R->get(i, j);
        return pixel(l.r - r.r, l.g - r.g, l.b - r.b, l.a - r.a);
    }

    virtual void doc(std::ostream& out) const
    {
        out << "difference";
    }
};

//------------------------------------------------------------------------------

/// Multiplication operator

class multiply : public image
{
public:
    /// Multiply all samples of image *L* and image *R*. This operator is short-
    /// circuited and will only evaluate *R* if *L* is non-zero. The width,
    /// height, and depth of the result are the larger of the widths, heights,
    /// and depths of the inputs.

    multiply(image *L, image *R) : image(L, R) { }

    virtual pixel get(int i, int j) const
    {
        pixel l = L->get(i, j);

        if (l.r || l.g || l.b || l.a)
        {
            pixel r = R->get(i, j);
            return pixel(l.r * r.r, l.g * r.g, l.b * r.b, l.a * r.a);
        }
        else return pixel();
    }

    virtual void doc(std::ostream& out) const
    {
        out << "multiply";
    }
};

//------------------------------------------------------------------------------

#endif
