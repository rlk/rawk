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

/// Solid color generator

class solid : public image
{
public:
    solid(int h, int w, double v) : h(h), w(w), value(v) { }

    virtual double get(int, int, int) const;

    virtual int geth() const { return h; }
    virtual int getw() const { return w; }

    virtual std::string doc() const;

private:
    int h;
    int w;
    double value;
};

//------------------------------------------------------------------------------

double solid::get(int i, int j, int k) const
{
    return value;
}

std::string solid::doc() const
{
    std::ostringstream sout;
    sout << "solid " << h <<
                 " " << w <<
                 " " << value;
    return sout.str();
}

//------------------------------------------------------------------------------

#endif
