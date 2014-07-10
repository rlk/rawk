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
    /// Blend image *L* with image *R* using the alpha channel of *L*.

    blend(image *L, image *R) : image(L, R) { }

    virtual pixel get(int i, int j) const
    {
        pixel l = L->get(i, j);
        pixel r = R->get(i, j);

        return pixel(l.a * l.r + (1.0 - l.a) * r.r,
                     l.a * l.g + (1.0 - l.a) * r.g,
                     l.a * l.b + (1.0 - l.a) * r.b,
                     l.a * l.a + (1.0 - l.a) * r.a);
    }

    virtual void doc(std::ostream& out) const
    {
        out << "blend";
    }
};

//------------------------------------------------------------------------------

#endif
