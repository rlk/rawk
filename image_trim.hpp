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

#ifndef IMAGE_TRIM_HPP
#define IMAGE_TRIM_HPP

//------------------------------------------------------------------------------

/// Trim filter

class trim : public image
{
public:
    /// Add or subtract equal amounts of the top and bottom or left and right
    /// edges of *L* to give a result with size *height* and *width*. This is
    /// akin to a ::crop filter that automatically calculates the row and column
    /// to favor the center of an image. If *height* and *width* are larger than
    /// the source, the result will depend upon how *L* is configured to handle
    /// out-of-bounds sampling.

    trim(int height, int width, image *L)
        : image(L), height(height), width(width) { }

    virtual double get(int i, int j, int k) const
    {
        const int h = L->get_height();
        const int w = L->get_width ();

        if (h > height) i += (h - height) / 2;
        if (height > h) i -= (height - h) / 2;
        if (w >  width) j += (w -  width) / 2;
        if (width  > w) j -= (width  - w) / 2;

        return L->get(i, j, k);
    }

    virtual int get_height() const { return height; }
    virtual int get_width () const { return width;  }

    virtual void tweak(int a, int v)
    {
        if (a == 0) width  += v;
        if (a == 1) height += v;
    }

    virtual void doc(std::ostream& out) const
    {
        out << "trim " << height << " " << width;
    }

private:
    int height;
    int width;
};

//------------------------------------------------------------------------------

#endif
