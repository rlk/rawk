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

#ifndef IMAGE_NEAREST_HPP
#define IMAGE_NEAREST_HPP

//------------------------------------------------------------------------------

/// Nearest value resampling filter

class nearest : public image
{
public:
    nearest(int height, int width, image *L)
        : image(L), height(height), width(width) { }

    virtual double get(int i, int j, int k) const
    {
        const long long hh = (long long) L->get_height();
        const long long ww = (long long) L->get_width();

        return L->get(int((long long) i * hh / (long long) height),
                      int((long long) j * ww / (long long) width), k);
    }

    virtual int get_height() const { return height; }
    virtual int get_width () const { return width;  }

    virtual void tweak(int a, int v)
    {
        if (a == 0) width -= v;
        if (a == 1) height -= v;
    }

    virtual void doc(std::ostream& out) const
    {
        out << "nearest " << height << " " << width;
    }

private:
    int height;
    int width;
};

//------------------------------------------------------------------------------

#endif
