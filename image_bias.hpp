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

#ifndef IMAGE_BIAS_HPP
#define IMAGE_BIAS_HPP

#include "image.hpp"

//------------------------------------------------------------------------------

/// Bias filter

class bias : public image
{
public:
    bias(double d, image *L) : image(L), value(d) { }

    virtual double get(int, int, int) const;
    virtual void tweak(int, int);

    virtual std::string doc() const;

private:
    double value;
};

double bias::get(int i, int j, int k) const
{
    return L->get(i, j, k) + value;
}

void bias::tweak(int a, int v)
{
    if (a == 0) value += 0.1 * v;
}

std::string bias::doc() const
{
    std::ostringstream sout;
    sout << "bias " << value;
    return sout.str();
}

//------------------------------------------------------------------------------

#endif
