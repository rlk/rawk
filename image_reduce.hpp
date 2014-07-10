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

/// 50% down-sample filter

class reduce : public image
{
public:
    /// Apply a 2-by-2 box-filtered down-sampling to image *L*. The height and
    /// width of the result will be exactly half that of the input. Reduction is
    /// often preferable to linear or even cubic downsampling as all samples of
    /// the input are guaranteed to have equal influence on the output.

    reduce(image *L) : image(L) { }

    virtual pixel get(int i, int j) const
    {
        pixel a = L->get(i * 2 + 0, j * 2 + 0);
        pixel b = L->get(i * 2 + 0, j * 2 + 1);
        pixel c = L->get(i * 2 + 1, j * 2 + 0);
        pixel d = L->get(i * 2 + 1, j * 2 + 1);

        return pixel((a.r + b.r + c.r + d.r) / 4.0,
                     (a.g + b.g + c.g + d.g) / 4.0,
                     (a.b + b.b + c.b + d.b) / 4.0,
                     (a.a + b.a + c.a + d.a) / 4.0);
    }

    virtual int get_height() const { return L->get_height() / 2; }
    virtual int get_width () const { return L->get_width () / 2; }

    virtual void doc(std::ostream& out) const
    {
        out << "reduce";
    }
};

//------------------------------------------------------------------------------

#endif
