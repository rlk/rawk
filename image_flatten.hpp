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

#include "image.hpp"

//------------------------------------------------------------------------------

/// Spherical flatten

class flatten : public image
{
public:
    flatten(double d, image *L) : image(L), value(d) { }

    virtual double get(int, int, int) const;
    virtual void tweak(int, int);

    virtual std::string doc() const;

private:
    double value;
};

double flatten::get(int i, int j, int k) const
{
    const int h = L->geth() / 2;

    double lat = M_PI_2 * double(h - i) / h;

    double y = sin(lat) * value;
    double x = cos(lat);
    double r = sqrt(x * x + y * y);

    x = x / r;
    y = y / r;

    lat = asin(y);

    i = -h * (lat - M_PI_2) / M_PI_2;

    return L->get(i, j, k);
}

void flatten::tweak(int a, int v)
{
    if (a == 0) value += 0.0001 * v;
}

std::string flatten::doc() const
{
    std::ostringstream sout;
    sout << "flatten " << value;
    return sout.str();
}

//------------------------------------------------------------------------------

#endif
