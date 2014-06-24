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

#ifndef IMAGE_GRADIENT_HPP
#define IMAGE_GRADIENT_HPP

#include "image.hpp"

//------------------------------------------------------------------------------

/// Gradient filter

class gradient : public image
{
public:
    gradient(int w, image *L) : image(L), mode(w) { }

    virtual double get(int, int, int) const;

    virtual std::string doc() const;

private:
    int mode;
};

double gradient::get(int ic, int jc, int k) const
{
    const int h = L->geth();
    const int w = L->getw();
    const int d = L->getd();

    if (0 <= ic && ic < h && 0 <= jc && jc < w && 0 <= k && k < d)
    {
        const int in = wrap(ic - 1, L->geth(), mode & 1);
        const int is = wrap(ic + 1, L->geth(), mode & 1);
        const int jw = wrap(jc - 1, L->getw(), mode & 2);
        const int je = wrap(jc + 1, L->getw(), mode & 2);

        double d1 = L->get(in, jw, k);
        double d2 = L->get(in, jc, k);
        double d3 = L->get(in, je, k);
        double d4 = L->get(ic, jw, k);

        double d6 = L->get(ic, je, k);
        double d7 = L->get(is, jw, k);
        double d8 = L->get(is, jc, k);
        double d9 = L->get(is, je, k);

        double Lx = d3 - d1 + 2.0 * (d6 - d4) + d9 - d7;
        double Ly = d1 - d7 + 2.0 * (d2 - d8) + d3 - d9;

        return sqrt(Lx * Lx + Ly * Ly);
    }
    return 0.0;
}

std::string gradient::doc() const
{
    std::ostringstream sout;
    sout << "gradient " << mode;
    return sout.str();
}

//------------------------------------------------------------------------------

#endif
