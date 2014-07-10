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

#ifndef IMAGE_SWIZZLE_HPP
#define IMAGE_SWIZZLE_HPP

//------------------------------------------------------------------------------

float channel(pixel p, char c)
{
    if (c == 'R') return p.r;
    if (c == 'G') return p.g;
    if (c == 'B') return p.b;
    if (c == 'A') return p.a;
    return 0;
}

/// Swizzle filter

class swizzle : public image
{
public:
    /// Reorder, replicate, or remove channels of *L*. *Element* is a string of
    /// channel names, (R, G, B, A).
    ///
    /// For example `BGR` would produce a BGR output from an RGB input. `G`
    /// would isolate the green channel. `RRR` would route the red channel to
    /// all three channels. `RGBG` would duplicate the green channel as an alpha
    /// channel.

    swizzle(std::string element, image *L) : image(L), element(element) { }

    virtual pixel get(int i, int j) const
    {
        pixel p = L->get(i, j);
        int d = element.size();

        float r = d > 0 ? channel(p, element[0]) : 0;
        float g = d > 1 ? channel(p, element[1]) : 0;
        float b = d > 2 ? channel(p, element[2]) : 0;
        float a = d > 3 ? channel(p, element[3]) : 0;

        return pixel(r, g, b, a);
    }

    virtual void doc(std::ostream& out) const
    {
        out << "swizzle " << element;
    }

private:
    std::string      element;
    std::vector<int> index;
};

//------------------------------------------------------------------------------

#endif
