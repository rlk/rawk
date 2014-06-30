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
    paste(int row, int column, image *L, image *R)
        : image(L, R), row(row), column(column) { }

    virtual double get(int i, int j, int k) const
    {
        if (row    <= i && i < row    + L->get_height() &&
            column <= j && j < column + L->get_width())

            return L->get(i - row, j - column, k);
        else
            return R->get(i, j, k);
    }

    virtual int get_height() const
    {
        return std::max(L->get_height() + row,    R->get_height());
    }

    virtual int get_width () const
    {
        return std::max(L->get_width()  + column, R->get_width());
    }

    virtual std::string doc() const
    {
        std::ostringstream out;
        out << "paste " << row << " " << column;
        return out.str();
    }

private:
    int row;
    int column;
};

//------------------------------------------------------------------------------

#endif
