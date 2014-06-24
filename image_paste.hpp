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

#ifndef IMAGE_PASTE_HPP
#define IMAGE_PASTE_HPP

//------------------------------------------------------------------------------

/// Overlay operator

class paste : public image
{
public:
    paste(int r, int c, image *L, image *R) : image(L, R), row(r), col(c) { }

    virtual double get(int, int, int) const;

    virtual int geth() const;
    virtual int getw() const;

    virtual std::string doc() const;

private:
    int row;
    int col;
};

//------------------------------------------------------------------------------

double paste::get(int i, int j, int k) const
{
    if (row <= i && i < row + L->geth() &&
        col <= j && j < col + L->getw())
        return L->get(i - row, j - col, k);
    else
        return R->get(i, j, k);
}

int paste::geth() const
{
    return std::max(L->geth() + row, R->geth());
}

int paste::getw() const
{
    return std::max(L->getw() + col, R->getw());
}

std::string paste::doc() const
{
    std::ostringstream sout;
    sout << "paste " << row
              << " " << col;
    return sout.str();
}

//------------------------------------------------------------------------------

#endif
