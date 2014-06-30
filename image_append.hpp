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

#ifndef IMAGE_APPEND_HPP
#define IMAGE_APPEND_HPP

//------------------------------------------------------------------------------

/// Channel append

class append : public image
{
public:
    append(image *L, image *R) : image(L, R) { }

    virtual double get(int i, int j, int k) const
    {
        const int d = L->get_depth();

        if (k < d)
            return L->get(i, j, k);
        else
            return R->get(i, j, k - d);
    }

    virtual int get_depth() const
    {
        return L->get_depth() + R->get_depth();
    }

    virtual std::string doc() const
    {
        return "append";
    }
};

//------------------------------------------------------------------------------

#endif
