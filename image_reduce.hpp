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

#ifndef IMAGE_REDUCE_HPP
#define IMAGE_REDUCE_HPP

//------------------------------------------------------------------------------

/// 50% downsample filter

class reduce : public image
{
public:
    reduce(image *L) : image(L) { }

    virtual double get(int i, int j, int k) const
    {
        return (L->get(i * 2 + 0, j * 2 + 0, k) +
                L->get(i * 2 + 0, j * 2 + 1, k) +
                L->get(i * 2 + 1, j * 2 + 0, k) +
                L->get(i * 2 + 1, j * 2 + 1, k)) / 4.0;
    }

    virtual int get_height() const { return L->get_height() / 2; }
    virtual int get_width () const { return L->get_width () / 2; }

    virtual std::string doc() const
    {
        return "reduce";
    }
};

//------------------------------------------------------------------------------

#endif
