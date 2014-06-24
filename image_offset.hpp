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

#ifndef IMAGE_OFFSETS_HPP
#define IMAGE_OFFSETS_HPP

//------------------------------------------------------------------------------

/// Offset filter

class offset : public image
{
public:
    offset(int r, int c, int m, image *L) : image(L), row(r), col(c), mode(m) { }

    virtual double get(int, int, int) const;
    virtual void tweak(int, int);

    virtual std::string doc() const;

private:
    int row;
    int col;
    int mode;
};

//------------------------------------------------------------------------------

double offset::get(int i, int j, int k) const
{
    const int h = L->geth();
    const int w = L->getw();
    const int d = L->getd();

    if (0 <= i && i < h && 0 <= j && j < w && 0 <= k && k < d)
        return L->get(wrap(i - row, h, mode & 1),
                      wrap(j - col, w, mode & 2), k);
    else
        return 0.0;
}

void offset::tweak(int a, int v)
{
    if (a == 0) col += v;
    if (a == 1) row += v;
}

std::string offset::doc() const
{
    std::ostringstream sout;
    sout << "offset " << row
               << " " << col
               << " " << mode;
    return sout.str();
}

//------------------------------------------------------------------------------

#endif
