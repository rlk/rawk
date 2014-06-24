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

#ifndef IMAGE_SUM_HPP
#define IMAGE_SUM_HPP

#include "image.hpp"

//------------------------------------------------------------------------------

/// Sum operator

class sum : public image
{
public:
    sum(image *L, image *R) : image(L, R) { }

    virtual double get(int, int, int) const;

    virtual std::string doc() const;
};

double sum::get(int i, int j, int k) const
{
    return L->get(i, j, k)
         + R->get(i, j, k);
}

std::string sum::doc() const
{
    std::ostringstream sout;
    sout << "sum";
    return sout.str();
}

//------------------------------------------------------------------------------

#endif
