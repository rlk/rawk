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

#ifndef IMAGE_FLATTEN_HPP
#define IMAGE_FLATTEN_HPP

//------------------------------------------------------------------------------

/// Spherical flatten
///
/// Account for variation in the flatness of the ellipse used to project
/// spherical data. This function takes integer pixel locations as both input
/// and output and performs no interpolation. This is preferable (for now) as
/// it preserves the identity of data dropouts.

class flatten : public image
{
public:
    flatten(double value, image *L) : image(L), value(value) { }

    virtual double get(int i, int j, int k) const
    {
        const int h = L->get_height() / 2;

        double l = M_PI_2 * double(h - i) / h;

        double y = sin(l) * value;
        double x = cos(l);
        double r = sqrt(x * x + y * y);

        return L->get(-h * (asin(y / r) - M_PI_2) / M_PI_2, j, k);
    }

    virtual void tweak(int a, int v)
    {
        if (a == 0) value += 0.0001 * v;
    }

    virtual std::string doc() const
    {
        std::ostringstream out;
        out << "flatten " << value;
        return out.str();
    }

private:
    double value;
};

//------------------------------------------------------------------------------

#endif
