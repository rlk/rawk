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

#ifndef IMAGE_FUNCTION_HPP
#define IMAGE_FUNCTION_HPP

//------------------------------------------------------------------------------

/// Absolute value filter

class absolute : public image
{
public:
    /// Give the absolute value of image *L*.

    absolute(image *L) : image(L) { }

    virtual double get(int i, int j, int k) const
    {
        return fabs(L->get(i, j, k));
    }

    virtual void doc(std::ostream& out) const
    {
        out << "absolute";
    }
};

//------------------------------------------------------------------------------

#endif
