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

/// Nearest filter

class nearest : public image
{
public:
    nearest(int h, int w, image *L) : image(L), h(h), w(w) { }

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

double nearest::get(int i, int j, int k) const
{
    const long long hh = (long long) L->geth();
    const long long ww = (long long) L->getw();

    return L->get(int((long long) i * hh / (long long) h),
                  int((long long) j * ww / (long long) w), k);
}

void nearest::tweak(int a, int v)
{
    if (a == 0) w -= v;
    if (a == 1) h -= v;
}

std::string nearest::doc() const
{
    std::ostringstream sout;
    sout << "nearest " << h
                << " " << w;
    return sout.str();
}

//------------------------------------------------------------------------------

#endif
