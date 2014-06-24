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

#ifndef IMAGE_SCALE_HPP
#define IMAGE_SCALE_HPP

#include "image.hpp"

//------------------------------------------------------------------------------

/// Scale filter

class scale : public image
{
public:
    scale(double d, image *L) : image(L), value(d) { }

    virtual double get(int, int, int) const;

    virtual std::string doc() const;

private:
    double value;
};

double scale::get(int i, int j, int k) const
{
    return L->get(i, j, k) * value;
}

std::string scale::doc() const
{
    std::ostringstream sout;
    sout << "scale " << value;
    return sout.str();
}

//------------------------------------------------------------------------------

#endif
