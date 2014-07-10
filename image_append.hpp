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
    /// Append the channels of image *R* to those of image *L*. This can be used
    /// to assemble a multichannel image from grayscale inputs, or to attach an
    /// alpha channel for use by ::blend. The width, height, and depth of the
    /// result are the larger of the widths, heights, and depths of the two
    /// inputs.

    append(image *L, image *R) : image(L, R) { }

    virtual pixel get(int i, int j) const
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

    virtual void doc(std::ostream& out) const
    {
        out << "append";
    }
};

//------------------------------------------------------------------------------

#endif
