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

#ifndef IMAGE_BLEND_HPP
#define IMAGE_BLEND_HPP

//------------------------------------------------------------------------------

/// Blend operator

class blend : public image
{
public:
    blend(image *L, image *R) : image(L, R) { }

    virtual double get(int i, int j, int k) const
    {
        double a = L->get(i, j, L->get_depth() - 1);

        if (a == 1.0) return L->get(i, j, k);
        if (a == 0.0) return R->get(i, j, k);

        return a * L->get(i, j, k) + (1.0 - a) * R->get(i, j, k);
    }

    virtual int get_depth() const
    {
        return std::max(L->get_depth() - 1, R->get_depth());
    }

    virtual void doc(std::ostream& out) const
    {
        out << "blend";
    }
};

//------------------------------------------------------------------------------

#endif
