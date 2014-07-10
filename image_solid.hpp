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

#ifndef IMAGE_SOLID_HPP
#define IMAGE_SOLID_HPP

//------------------------------------------------------------------------------

/// Solid value generator

class solid : public image
{
public:
    /// Generate a block of pixels with the given *width* and *height* and
    /// color *r*, *g*, *b*, *a*.

    solid(int height, int width, float r, float g, float b, float a)
        : height(height), width(width), p(r, g, b, a) { }

    virtual pixel get(int i, int j) const
    {
        return p;
    }

    virtual int get_height() const { return height; }
    virtual int get_width () const { return width;  }

    virtual void doc(std::ostream& out) const
    {
        out << "solid " << height
                 << " " << width
                 << " " << p.r
                 << " " << p.g
                 << " " << p.b
                 << " " << p.a;
    }

private:
    int   height;
    int   width;
    pixel p;
};

//------------------------------------------------------------------------------

#endif
