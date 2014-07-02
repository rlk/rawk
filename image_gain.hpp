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

#ifndef IMAGE_GAIN_HPP
#define IMAGE_GAIN_HPP

//------------------------------------------------------------------------------

/// Gain filter

class gain : public image
{
public:
    /// Multiply *value* by all samples of *L*. This alters the contrast of *L*.

    gain(double value, image *L) : image(L), value(value) { }

    virtual double get(int i, int j, int k) const
    {
        return L->get(i, j, k) * value;
    }

    virtual void tweak(int a, int v)
    {
        if (a == 0) value += 0.1 * v;
    }

    virtual void doc(std::ostream& out) const
    {
        out << "gain " << value;
    }

private:
    double value;
};

//------------------------------------------------------------------------------

#endif
