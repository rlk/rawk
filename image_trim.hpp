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
    trim(int h, int w, image *L) : image(L), h(h), w(w) { }

    virtual double get(int, int, int) const;
    virtual void tweak(int, int);

    virtual int geth() const { return h; }
    virtual int getw() const { return w; }

    virtual std::string doc() const;

private:
    int h;
    int w;
};

//------------------------------------------------------------------------------

double trim::get(int i, int j, int k) const
{
    const int hh = L->geth();
    const int ww = L->getw();

    if (hh > h) i += (hh - h) / 2;
    if (h > hh) i -= (h - hh) / 2;
    if (ww > w) j += (ww - w) / 2;
    if (w > ww) j -= (w - ww) / 2;

    return L->get(i, j, k);
}

void trim::tweak(int a, int v)
{
    if (a == 0) w += v;
    if (a == 1) h += v;
}

std::string trim::doc() const
{
    std::ostringstream sout;
    sout << "trim " << h
             << " " << w;
    return sout.str();
}

//------------------------------------------------------------------------------

#endif
