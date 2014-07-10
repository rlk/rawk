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
    /// Paste image *L* over image *R*. *Row* and *column* give the position of
    /// the top-left corner of the pasted image within the underlying image.
    /// This is useful for reconstructing tiled or sub-divided image files. The
    /// width and height of the result expand to accommodate the sizes of both
    /// source images.

    paste(int row, int column, image *L, image *R)
        : image(L, R), row(row), column(column) { }

    virtual pixel get(int i, int j) const
    {
        if (row    <= i && i < row    + L->get_height() &&
            column <= j && j < column + L->get_width())

            return L->get(i - row, j - column);
        else
            return R->get(i, j);
    }

    virtual int get_height() const
    {
        return std::max(L->get_height() + row,    R->get_height());
    }

    virtual int get_width () const
    {
        return std::max(L->get_width()  + column, R->get_width());
    }

    virtual void doc(std::ostream& out) const
    {
        out << "paste " << row << " " << column;
    }

private:
    int row;
    int column;
};

//------------------------------------------------------------------------------

#endif
